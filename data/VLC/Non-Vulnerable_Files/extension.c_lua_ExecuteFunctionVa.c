 **/
int lua_ExecuteFunctionVa( extensions_manager_t *p_mgr, extension_t *p_ext,
                           const char *psz_function, va_list args )
{
    int i_ret = VLC_SUCCESS;
    int i_args = 0;
    assert( p_mgr != NULL );
    assert( p_ext != NULL );
    lua_State *L = GetLuaState( p_mgr, p_ext );
    if( !L )
        return -1;
    if( psz_function )
        lua_getglobal( L, psz_function );
    if( !lua_isfunction( L, -1 ) )
    {
        msg_Warn( p_mgr, "Error while running script %s, "
                  "function %s() not found", p_ext->psz_name, psz_function );
        goto exit;
    }
    lua_datatype_e type = LUA_END;
    while( ( type = va_arg( args, int ) ) != LUA_END )
    {
        if( type == LUA_NUM )
        {
            lua_pushnumber( L , ( int ) va_arg( args, int ) );
        }
        else if( type == LUA_TEXT )
        {
            lua_pushstring( L , ( char * ) va_arg( args, char* ) );
        }
        else
        {
            msg_Warn( p_mgr, "Undefined argument type %d to lua function %s"
                   "from script %s", type, psz_function, p_ext->psz_name );
            goto exit;
        }
        i_args ++;
    }
    // Create watch timer
    vlc_mutex_lock( &p_ext->p_sys->command_lock );
    vlc_timer_schedule( p_ext->p_sys->timer, false, WATCH_TIMER_PERIOD, 0 );
    vlc_mutex_unlock( &p_ext->p_sys->command_lock );
    // Start actual call to Lua
    if( lua_pcall( L, i_args, 1, 0 ) )
    {
        msg_Warn( p_mgr, "Error while running script %s, "
                  "function %s(): %s", p_ext->psz_name, psz_function,
                  lua_tostring( L, lua_gettop( L ) ) );
        i_ret = VLC_EGENERIC;
    }
    // Reset watch timer and timestamp
    vlc_mutex_lock( &p_ext->p_sys->command_lock );
    if( p_ext->p_sys->progress )
    {
        dialog_ProgressDestroy( p_ext->p_sys->progress );
        p_ext->p_sys->progress = NULL;
    }
    vlc_timer_schedule( p_ext->p_sys->timer, false, 0, 0 );
    vlc_mutex_unlock( &p_ext->p_sys->command_lock );
    i_ret |= lua_DialogFlush( L );
exit:
    return i_ret;
}
