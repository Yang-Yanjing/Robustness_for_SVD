}
void RtspUnsetup( rtsp_stream_t *rtsp )
{
    if( rtsp->url )
        httpd_UrlDelete( rtsp->url );
    if( rtsp->host )
        httpd_HostDelete( rtsp->host );
    while( rtsp->sessionc > 0 )
        RtspClientDel( rtsp, rtsp->sessionv[0] );
    if (rtsp->timeout > 0)
        vlc_timer_destroy(rtsp->timer);
    free( rtsp->psz_path );
    vlc_mutex_destroy( &rtsp->lock );
    free( rtsp );
}
