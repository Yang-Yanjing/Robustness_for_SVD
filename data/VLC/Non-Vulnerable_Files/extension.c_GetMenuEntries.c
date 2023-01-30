 **/
static int GetMenuEntries( extensions_manager_t *p_mgr, extension_t *p_ext,
                    char ***pppsz_titles, uint16_t **ppi_ids )
{
    assert( *pppsz_titles == NULL );
    assert( *ppi_ids == NULL );
    if( !IsActivated( p_mgr, p_ext ) )
    {
        msg_Dbg( p_mgr, "Can't get menu before activating the extension!" );
        return VLC_EGENERIC;
    }
    if( !LockExtension( p_ext ) )
    {
        /* Dying extension, fail. */
        return VLC_EGENERIC;
    }
    int i_ret = VLC_EGENERIC;
    lua_State *L = GetLuaState( p_mgr, p_ext );
    if( ( p_ext->p_sys->i_capabilities & EXT_HAS_MENU ) == 0 )
    {
        msg_Dbg( p_mgr, "can't get a menu from an extension without menu!" );
        goto exit;
    }
    lua_getglobal( L, "menu" );
    if( !lua_isfunction( L, -1 ) )
    {
        msg_Warn( p_mgr, "Error while running script %s, "
                  "function menu() not found", p_ext->psz_name );
        goto exit;
    }
    if( lua_pcall( L, 0, 1, 0 ) )
    {
        msg_Warn( p_mgr, "Error while running script %s, "
                  "function menu(): %s", p_ext->psz_name,
                  lua_tostring( L, lua_gettop( L ) ) );
        goto exit;
    }
    if( lua_gettop( L ) )
    {
        if( lua_istable( L, -1 ) )
        {
            /* Get table size */
            size_t i_size = lua_objlen( L, -1 );
            *pppsz_titles = ( char** ) calloc( i_size+1, sizeof( char* ) );
            *ppi_ids = ( uint16_t* ) calloc( i_size+1, sizeof( uint16_t ) );
            /* Walk table */
            size_t i_idx = 0;
            lua_pushnil( L );
            while( lua_next( L, -2 ) != 0 )
            {
                assert( i_idx < i_size );
                if( (!lua_isstring( L, -1 )) || (!lua_isnumber( L, -2 )) )
                {
                    msg_Warn( p_mgr, "In script %s, an entry in "
                              "the menu table is invalid!", p_ext->psz_name );
                    goto exit;
                }
                (*pppsz_titles)[ i_idx ] = strdup( luaL_checkstring( L, -1 ) );
                (*ppi_ids)[ i_idx ] = (uint16_t) ( luaL_checkinteger( L, -2 ) & 0xFFFF );
                i_idx++;
                lua_pop( L, 1 );
            }
        }
        else
        {
            msg_Warn( p_mgr, "Function menu() in script %s "
                      "did not return a table", p_ext->psz_name );
            goto exit;
        }
    }
    else
    {
        msg_Warn( p_mgr, "Script %s went completely foobar", p_ext->psz_name );
        goto exit;
    }
    i_ret = VLC_SUCCESS;
exit:
    UnlockExtension( p_ext );
    if( i_ret != VLC_SUCCESS )
    {
        msg_Dbg( p_mgr, "Something went wrong in %s (%s:%d)",
                 __func__, __FILE__, __LINE__ );
    }
    return i_ret;
}
