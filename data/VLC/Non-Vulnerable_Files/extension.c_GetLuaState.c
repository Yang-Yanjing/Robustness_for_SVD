/* Must be entered with the Lock on Extension */
static lua_State* GetLuaState( extensions_manager_t *p_mgr,
                               extension_t *p_ext )
{
    lua_State *L = NULL;
    if( p_ext )
        L = p_ext->p_sys->L;
    if( !L )
    {
        L = luaL_newstate();
        if( !L )
        {
            msg_Err( p_mgr, "Could not create new Lua State" );
            return NULL;
        }
        vlclua_set_this( L, p_mgr );
        vlclua_set_playlist_internal( L,
            pl_Get((intf_thread_t *)(p_mgr->p_parent)) );
        vlclua_extension_set( L, p_ext );
        luaL_openlibs( L );
        luaL_register( L, "vlc", p_reg );
        luaopen_msg( L );
        if( p_ext )
        {
            /* Load more libraries */
            luaopen_config( L );
            luaopen_dialog( L, p_ext );
            luaopen_input( L );
            luaopen_msg( L );
            if( vlclua_fd_init( L, &p_ext->p_sys->dtable ) )
            {
                lua_close( L );
                return NULL;
            }
            luaopen_object( L );
            luaopen_osd( L );
            luaopen_playlist( L );
            luaopen_sd_intf( L );
            luaopen_stream( L );
            luaopen_strings( L );
            luaopen_variables( L );
            luaopen_video( L );
            luaopen_vlm( L );
            luaopen_volume( L );
            luaopen_xml( L );
#if defined(_WIN32) && !VLC_WINSTORE_APP
            luaopen_win( L );
#endif
            /* Register extension specific functions */
            lua_getglobal( L, "vlc" );
            lua_pushcfunction( L, vlclua_extension_deactivate );
            lua_setfield( L, -2, "deactivate" );
            lua_pushcfunction( L, vlclua_extension_keep_alive );
            lua_setfield( L, -2, "keep_alive" );
            /* Setup the module search path */
            if( !strncmp( p_ext->psz_name, "zip://", 6 ) )
            {
                /* Load all required modules manually */
                lua_register( L, "require", &vlclua_extension_require );
            }
            else
            {
                if( vlclua_add_modules_path( L, p_ext->psz_name ) )
                {
                    msg_Warn( p_mgr, "Error while setting the module "
                              "search path for %s", p_ext->psz_name );
                    vlclua_fd_cleanup( &p_ext->p_sys->dtable );
                    lua_close( L );
                    return NULL;
                }
            }
            /* Load and run the script(s) */
            if( vlclua_dofile( VLC_OBJECT( p_mgr ), L, p_ext->psz_name ) )
            {
                msg_Warn( p_mgr, "Error loading script %s: %s", p_ext->psz_name,
                          lua_tostring( L, lua_gettop( L ) ) );
                vlclua_fd_cleanup( &p_ext->p_sys->dtable );
                lua_close( L );
                return NULL;
            }
            p_ext->p_sys->L = L;
        }
    }
    return L;
}
