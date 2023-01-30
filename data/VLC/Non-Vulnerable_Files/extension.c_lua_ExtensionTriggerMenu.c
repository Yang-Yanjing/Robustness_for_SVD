}
int lua_ExtensionTriggerMenu( extensions_manager_t *p_mgr,
                              extension_t *p_ext, int id )
{
    int i_ret = VLC_SUCCESS;
    lua_State *L = GetLuaState( p_mgr, p_ext );
    if( !L )
        return VLC_EGENERIC;
    luaopen_dialog( L, p_ext );
    lua_getglobal( L, "trigger_menu" );
    if( !lua_isfunction( L, -1 ) )
    {
        msg_Warn( p_mgr, "Error while running script %s, "
                  "function trigger_menu() not found", p_ext->psz_name );
        return VLC_EGENERIC;
    }
    /* Pass id as unique argument to the function */
    lua_pushinteger( L, id );
    // Create watch timer
    vlc_mutex_lock( &p_ext->p_sys->command_lock );
    vlc_timer_schedule( p_ext->p_sys->timer, false, WATCH_TIMER_PERIOD, 0 );
    vlc_mutex_unlock( &p_ext->p_sys->command_lock );
    if( lua_pcall( L, 1, 1, 0 ) != 0 )
    {
        msg_Warn( p_mgr, "Error while running script %s, "
                  "function trigger_menu(): %s", p_ext->psz_name,
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
    if( i_ret < VLC_SUCCESS )
    {
        msg_Dbg( p_mgr, "Something went wrong in %s (%s:%d)",
                 __func__, __FILE__, __LINE__ );
    }
    return i_ret;
}
