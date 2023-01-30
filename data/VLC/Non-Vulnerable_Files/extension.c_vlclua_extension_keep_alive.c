 **/
int vlclua_extension_keep_alive( lua_State *L )
{
    extension_t *p_ext = vlclua_extension_get( L );
    vlc_mutex_lock( &p_ext->p_sys->command_lock );
    if( p_ext->p_sys->progress )
    {
        dialog_ProgressDestroy( p_ext->p_sys->progress );
        p_ext->p_sys->progress = NULL;
    }
    vlc_timer_schedule( p_ext->p_sys->timer, false, WATCH_TIMER_PERIOD, 0 );
    vlc_mutex_unlock( &p_ext->p_sys->command_lock );
    return 1;
}
