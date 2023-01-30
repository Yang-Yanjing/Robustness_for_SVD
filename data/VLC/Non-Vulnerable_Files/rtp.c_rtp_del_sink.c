}
void rtp_del_sink( sout_stream_id_sys_t *id, int fd )
{
    rtp_sink_t sink = { fd, NULL };
    /* NOTE: must be safe to use if fd is not included */
    vlc_mutex_lock( &id->lock_sink );
    for( int i = 0; i < id->sinkc; i++ )
    {
        if (id->sinkv[i].rtp_fd == fd)
        {
            sink = id->sinkv[i];
            REMOVE_ELEM( id->sinkv, id->sinkc, i );
            break;
        }
    }
    vlc_mutex_unlock( &id->lock_sink );
    CloseRTCP( sink.rtcp );
    net_Close( sink.rtp_fd );
}
