 **/
int ScanLuaCallback( vlc_object_t *p_this, const char *psz_filename,
                     const struct luabatch_context_t *dummy )
{
    VLC_UNUSED(dummy);
    extensions_manager_t *p_mgr = ( extensions_manager_t* ) p_this;
    bool b_ok = false;
    msg_Dbg( p_mgr, "Scanning Lua script %s", psz_filename );
    /* Experimental: read .vle packages (Zip archives) */
    char *psz_script = NULL;
    int i_flen = strlen( psz_filename );
    if( !strncasecmp( psz_filename + i_flen - 4, ".vle", 4 ) )
    {
        msg_Dbg( p_this, "reading Lua script in a zip archive" );
        psz_script = calloc( 1, i_flen + 6 + 12 + 1 );
        if( !psz_script )
            return 0;
        strcpy( psz_script, "zip://" );
        strncat( psz_script, psz_filename, i_flen + 19 );
        strncat( psz_script, "!/script.lua", i_flen + 19 );
    }
    else
    {
        psz_script = strdup( psz_filename );
        if( !psz_script )
            return 0;
    }
    /* Create new script descriptor */
    extension_t *p_ext = ( extension_t* ) calloc( 1, sizeof( extension_t ) );
    if( !p_ext )
    {
        free( psz_script );
        return 0;
    }
    p_ext->psz_name = psz_script;
    p_ext->p_sys = (extension_sys_t*) calloc( 1, sizeof( extension_sys_t ) );
    if( !p_ext->p_sys || !p_ext->psz_name )
    {
        free( p_ext->psz_name );
        free( p_ext->p_sys );
        free( p_ext );
        return 0;
    }
    p_ext->p_sys->p_mgr = p_mgr;
    /* Watch timer */
    if( vlc_timer_create( &p_ext->p_sys->timer, WatchTimerCallback, p_ext ) )
    {
        free( p_ext->psz_name );
        free( p_ext->p_sys );
        free( p_ext );
        return 0;
    }
    /* Mutexes and conditions */
    vlc_mutex_init( &p_ext->p_sys->command_lock );
    vlc_mutex_init( &p_ext->p_sys->running_lock );
    vlc_cond_init( &p_ext->p_sys->wait );
    /* Prepare Lua state */
    lua_State *L = luaL_newstate();
    lua_register( L, "require", &vlclua_dummy_require );
    /* Let's run it */
    if( vlclua_dofile( p_this, L, psz_script ) ) // luaL_dofile
    {
        msg_Warn( p_mgr, "Error loading script %s: %s", psz_script,
                  lua_tostring( L, lua_gettop( L ) ) );
        lua_pop( L, 1 );
        goto exit;
    }
    /* Scan script for capabilities */
    lua_getglobal( L, "descriptor" );
    if( !lua_isfunction( L, -1 ) )
    {
        msg_Warn( p_mgr, "Error while running script %s, "
                  "function descriptor() not found", psz_script );
        goto exit;
    }
    if( lua_pcall( L, 0, 1, 0 ) )
    {
        msg_Warn( p_mgr, "Error while running script %s, "
                  "function descriptor(): %s", psz_script,
                  lua_tostring( L, lua_gettop( L ) ) );
        goto exit;
    }
    if( lua_gettop( L ) )
    {
        if( lua_istable( L, -1 ) )
        {
            /* Get caps */
            lua_getfield( L, -1, "capabilities" );
            if( lua_istable( L, -1 ) )
            {
                lua_pushnil( L );
                while( lua_next( L, -2 ) != 0 )
                {
                    /* Key is at index -2 and value at index -1. Discard key */
                    const char *psz_cap = luaL_checkstring( L, -1 );
                    bool b_ok = false;
                    /* Find this capability's flag */
                    for( size_t i = 0; i < sizeof(caps)/sizeof(caps[0]); i++ )
                    {
                        if( !strcmp( caps[i], psz_cap ) )
                        {
                            /* Flag it! */
                            p_ext->p_sys->i_capabilities |= 1 << i;
                            b_ok = true;
                            break;
                        }
                    }
                    if( !b_ok )
                    {
                        msg_Warn( p_mgr, "Extension capability '%s' unknown in"
                                  " script %s", psz_cap, psz_script );
                    }
                    /* Removes 'value'; keeps 'key' for next iteration */
                    lua_pop( L, 1 );
                }
            }
            else
            {
                msg_Warn( p_mgr, "In script %s, function descriptor() "
                              "did not return a table of capabilities.",
                              psz_script );
            }
            lua_pop( L, 1 );
            /* Get title */
            lua_getfield( L, -1, "title" );
            if( lua_isstring( L, -1 ) )
            {
                p_ext->psz_title = strdup( luaL_checkstring( L, -1 ) );
            }
            else
            {
                msg_Dbg( p_mgr, "In script %s, function descriptor() "
                                "did not return a string as title.",
                                psz_script );
                p_ext->psz_title = strdup( psz_script );
            }
            lua_pop( L, 1 );
            /* Get author */
            lua_getfield( L, -1, "author" );
            p_ext->psz_author = luaL_strdupornull( L, -1 );
            lua_pop( L, 1 );
            /* Get description */
            lua_getfield( L, -1, "description" );
            p_ext->psz_description = luaL_strdupornull( L, -1 );
            lua_pop( L, 1 );
            /* Get short description */
            lua_getfield( L, -1, "shortdesc" );
            p_ext->psz_shortdescription = luaL_strdupornull( L, -1 );
            lua_pop( L, 1 );
            /* Get URL */
            lua_getfield( L, -1, "url" );
            p_ext->psz_url = luaL_strdupornull( L, -1 );
            lua_pop( L, 1 );
            /* Get version */
            lua_getfield( L, -1, "version" );
            p_ext->psz_version = luaL_strdupornull( L, -1 );
            lua_pop( L, 1 );
            /* Get icon data */
            lua_getfield( L, -1, "icon" );
            if( !lua_isnil( L, -1 ) && lua_isstring( L, -1 ) )
            {
                int len = lua_strlen( L, -1 );
                p_ext->p_icondata = malloc( len );
                if( p_ext->p_icondata )
                {
                    p_ext->i_icondata_size = len;
                    memcpy( p_ext->p_icondata, lua_tostring( L, -1 ), len );
                }
            }
            lua_pop( L, 1 );
        }
        else
        {
            msg_Warn( p_mgr, "In script %s, function descriptor() "
                      "did not return a table!", psz_script );
            goto exit;
        }
    }
    else
    {
        msg_Err( p_mgr, "Script %s went completely foobar", psz_script );
        goto exit;
    }
    msg_Dbg( p_mgr, "Script %s has the following capability flags: 0x%x",
             psz_script, p_ext->p_sys->i_capabilities );
    b_ok = true;
exit:
    lua_close( L );
    if( !b_ok )
    {
        free( p_ext->psz_name );
        free( p_ext->psz_title );
        free( p_ext->psz_url );
        free( p_ext->psz_author );
        free( p_ext->psz_description );
        free( p_ext->psz_shortdescription );
        free( p_ext->psz_version );
        vlc_mutex_destroy( &p_ext->p_sys->command_lock );
        vlc_mutex_destroy( &p_ext->p_sys->running_lock );
        vlc_cond_destroy( &p_ext->p_sys->wait );
        free( p_ext->p_sys );
        free( p_ext );
    }
    else
    {
        /* Add the extension to the list of known extensions */
        ARRAY_APPEND( p_mgr->extensions, p_ext );
    }
    /* Continue batch execution */
    return VLC_EGENERIC;
}
