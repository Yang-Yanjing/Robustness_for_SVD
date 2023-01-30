}
static void RateChanged( intf_thread_t *p_intf,
                         input_thread_t *p_input )
{
    vlc_mutex_lock( &p_intf->p_sys->status_lock );
    msg_rc( STATUS_CHANGE "( new rate: %.3f )",
            var_GetFloat( p_input, "rate" ) );
    vlc_mutex_unlock( &p_intf->p_sys->status_lock );
}
