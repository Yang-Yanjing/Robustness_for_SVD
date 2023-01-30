}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    vlc_mutex_lock( &p_sys->lock_es );
    TAB_REMOVE( p_sys->i_es, p_sys->es, id );
    vlc_mutex_unlock( &p_sys->lock_es );
    if( likely(id->p_fifo != NULL) )
    {
        vlc_cancel( id->thread );
        vlc_join( id->thread, NULL );
        block_FifoRelease( id->p_fifo );
    }
    free( id->rtp_fmt.fmtp );
    if (p_sys->p_vod_media != NULL)
        vod_detach_id(p_sys->p_vod_media, p_sys->psz_vod_session, id);
    if( id->rtsp_id )
        RtspDelId( p_sys->rtsp, id->rtsp_id );
    if( id->listen.fd != NULL )
    {
        vlc_cancel( id->listen.thread );
        vlc_join( id->listen.thread, NULL );
        net_ListenClose( id->listen.fd );
    }
    /* Delete remaining sinks (incoming connections or explicit
     * outgoing dst=) */
    while( id->sinkc > 0 )
        rtp_del_sink( id, id->sinkv[0].rtp_fd );
#ifdef HAVE_SRTP
    if( id->srtp != NULL )
        srtp_destroy( id->srtp );
#endif
    vlc_mutex_destroy( &id->lock_sink );
    /* Update SDP (sap/file) */
    if( p_sys->b_export_sap ) SapSetup( p_stream );
    if( p_sys->psz_sdp_file != NULL ) FileSetup( p_stream );
    free( id );
    return VLC_SUCCESS;
}
