}
static void* Raw1394EventThread( void *obj )
{
    event_thread_t *p_ev = (event_thread_t *)obj;
    access_t *p_access = (access_t *) p_ev->p_access;
    access_sys_t *p_sys = (access_sys_t *) p_access->p_sys;
    int result = 0;
    int canc = vlc_savecancel();
    AVCPlay( p_access, p_sys->i_node );
    vlc_cleanup_push( Raw1394EventThreadCleanup, p_ev );
    vlc_restorecancel( canc );
    for( ;; )
    {
        while( ( result = poll( &p_sys->raw1394_poll, 1, -1 ) ) < 0 )
        {
            if( errno != EINTR )
                msg_Err( p_access, "poll error: %s", vlc_strerror_c(errno) );
        }
        if( result > 0 && ( ( p_sys->raw1394_poll.revents & POLLIN )
                         || ( p_sys->raw1394_poll.revents & POLLPRI ) ) )
        {
            canc = vlc_savecancel();
            result = raw1394_loop_iterate( p_sys->p_raw1394 );
            vlc_restorecancel( canc );
        }
    }
    vlc_cleanup_run();
    return NULL;
}
