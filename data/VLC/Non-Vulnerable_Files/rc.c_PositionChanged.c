}
static void PositionChanged( intf_thread_t *p_intf,
                             input_thread_t *p_input )
{
    vlc_mutex_lock( &p_intf->p_sys->status_lock );
    if( p_intf->p_sys->b_input_buffering )
        msg_rc( STATUS_CHANGE "( time: %"PRId64"s )",
                (var_GetTime( p_input, "time" )/1000000) );
    p_intf->p_sys->b_input_buffering = false;
    vlc_mutex_unlock( &p_intf->p_sys->status_lock );
}
