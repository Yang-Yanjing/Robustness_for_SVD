}
int rtp_add_sink( sout_stream_id_sys_t *id, int fd, bool rtcp_mux, uint16_t *seq )
{
    rtp_sink_t sink = { fd, NULL };
    sink.rtcp = OpenRTCP( VLC_OBJECT( id->p_stream ), fd, IPPROTO_UDP,
                          rtcp_mux );
    if( sink.rtcp == NULL )
        msg_Err( id->p_stream, "RTCP failed!" );
    vlc_mutex_lock( &id->lock_sink );
    INSERT_ELEM( id->sinkv, id->sinkc, id->sinkc, sink );
    if( seq != NULL )
        *seq = id->i_seq_sent_next;
    vlc_mutex_unlock( &id->lock_sink );
    return VLC_SUCCESS;
}
