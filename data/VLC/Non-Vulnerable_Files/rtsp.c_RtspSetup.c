static void RtspTimeOut( void *data );
rtsp_stream_t *RtspSetup( vlc_object_t *owner, vod_media_t *media,
                          const char *path )
{
    rtsp_stream_t *rtsp = malloc( sizeof( *rtsp ) );
    if( rtsp == NULL )
    {
        free( rtsp );
        return NULL;
    }
    rtsp->owner = owner;
    rtsp->vod_media = media;
    rtsp->sessionc = 0;
    rtsp->sessionv = NULL;
    rtsp->host = NULL;
    rtsp->url = NULL;
    rtsp->psz_path = NULL;
    rtsp->track_id = 0;
    vlc_mutex_init( &rtsp->lock );
    rtsp->timeout = var_InheritInteger(owner, "rtsp-timeout");
    if (rtsp->timeout > 0)
    {
        if (vlc_timer_create(&rtsp->timer, RtspTimeOut, rtsp))
            goto error;
    }
    rtsp->psz_path = strdup( (path != NULL) ? path : "/" );
    if( rtsp->psz_path == NULL )
        goto error;
    msg_Dbg( owner, "RTSP stream at %s", rtsp->psz_path );
    rtsp->host = vlc_rtsp_HostNew( VLC_OBJECT(owner) );
    if( rtsp->host == NULL )
        goto error;
    char *user = var_InheritString(owner, "sout-rtsp-user");
    char *pwd = var_InheritString(owner, "sout-rtsp-pwd");
    rtsp->url = httpd_UrlNew( rtsp->host, rtsp->psz_path, user, pwd );
    free(user);
    free(pwd);
    if( rtsp->url == NULL )
        goto error;
    httpd_UrlCatch( rtsp->url, HTTPD_MSG_DESCRIBE, RtspCallback, (void*)rtsp );
    httpd_UrlCatch( rtsp->url, HTTPD_MSG_SETUP,    RtspCallback, (void*)rtsp );
    httpd_UrlCatch( rtsp->url, HTTPD_MSG_PLAY,     RtspCallback, (void*)rtsp );
    httpd_UrlCatch( rtsp->url, HTTPD_MSG_PAUSE,    RtspCallback, (void*)rtsp );
    httpd_UrlCatch( rtsp->url, HTTPD_MSG_GETPARAMETER, RtspCallback,
                    (void*)rtsp );
    httpd_UrlCatch( rtsp->url, HTTPD_MSG_TEARDOWN, RtspCallback, (void*)rtsp );
    return rtsp;
error:
    RtspUnsetup( rtsp );
    return NULL;
}
