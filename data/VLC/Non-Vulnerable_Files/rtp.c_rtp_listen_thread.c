/* This thread dequeues incoming connections (DCCP streaming) */
static void *rtp_listen_thread( void *data )
{
    sout_stream_id_sys_t *id = data;
    assert( id->listen.fd != NULL );
    for( ;; )
    {
        int fd = net_Accept( id->p_stream, id->listen.fd );
        if( fd == -1 )
            continue;
        int canc = vlc_savecancel( );
        rtp_add_sink( id, fd, true, NULL );
        vlc_restorecancel( canc );
    }
    assert( 0 );
}
