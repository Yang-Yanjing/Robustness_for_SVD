}
static void CacheChanged( intf_thread_t *p_intf )
{
    vlc_mutex_lock( &p_intf->p_sys->status_lock );
    p_intf->p_sys->b_input_buffering = true;
    vlc_mutex_unlock( &p_intf->p_sys->status_lock );
}
