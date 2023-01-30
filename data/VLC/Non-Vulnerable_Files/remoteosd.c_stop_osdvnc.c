}
static void stop_osdvnc ( filter_t *p_filter )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    msg_Dbg( p_filter, "joining worker_thread" );
    vlc_cancel( p_sys->worker_thread );
    vlc_join( p_sys->worker_thread, NULL );
    msg_Dbg( p_filter, "released worker_thread" );
    msg_Dbg( p_filter, "osdvnc stopped" );
}
