 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if( p_sys->p_mux )
    {
        assert( p_sys->i_es <= 1 );
        sout_MuxDelete( p_sys->p_mux );
        if ( p_sys->i_es > 0 )
            Del( p_stream, p_sys->es[0] );
        sout_AccessOutDelete( p_sys->p_grab );
        if( p_sys->packet )
        {
            block_Release( p_sys->packet );
        }
    }
    if( p_sys->rtsp != NULL )
        RtspUnsetup( p_sys->rtsp );
    vlc_mutex_destroy( &p_sys->lock_sdp );
    vlc_mutex_destroy( &p_sys->lock_ts );
    vlc_mutex_destroy( &p_sys->lock_es );
    if( p_sys->p_httpd_file )
        httpd_FileDelete( p_sys->p_httpd_file );
    if( p_sys->p_httpd_host )
        httpd_HostDelete( p_sys->p_httpd_host );
    free( p_sys->psz_sdp );
    if( p_sys->psz_sdp_file != NULL )
    {
        unlink( p_sys->psz_sdp_file );
        free( p_sys->psz_sdp_file );
    }
    free( p_sys->psz_vod_session );
    free( p_sys->psz_destination );
    free( p_sys );
}
