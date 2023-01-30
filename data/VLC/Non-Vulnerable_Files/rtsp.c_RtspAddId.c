}
rtsp_stream_id_t *RtspAddId( rtsp_stream_t *rtsp, sout_stream_id_sys_t *sid,
                             uint32_t ssrc, unsigned clock_rate,
                             int mcast_fd)
{
    if (rtsp->track_id > 999)
    {
        msg_Err(rtsp->owner, "RTSP: too many IDs!");
        return NULL;
    }
    char *urlbuf;
    rtsp_stream_id_t *id = malloc( sizeof( *id ) );
    httpd_url_t *url;
    if( id == NULL )
        return NULL;
    id->stream = rtsp;
    id->sout_id = sid;
    id->track_id = rtsp->track_id;
    id->ssrc = ssrc;
    id->clock_rate = clock_rate;
    id->mcast_fd = mcast_fd;
    urlbuf = RtspAppendTrackPath( id, rtsp->psz_path );
    if( urlbuf == NULL )
    {
        free( id );
        return NULL;
    }
    msg_Dbg( rtsp->owner, "RTSP: adding %s", urlbuf );
    char *user = var_InheritString(rtsp->owner, "sout-rtsp-user");
    char *pwd = var_InheritString(rtsp->owner, "sout-rtsp-pwd");
    url = id->url = httpd_UrlNew( rtsp->host, urlbuf, user, pwd );
    free( user );
    free( pwd );
    free( urlbuf );
    if( url == NULL )
    {
        free( id );
        return NULL;
    }
    httpd_UrlCatch( url, HTTPD_MSG_DESCRIBE, RtspCallbackId, (void *)id );
    httpd_UrlCatch( url, HTTPD_MSG_SETUP,    RtspCallbackId, (void *)id );
    httpd_UrlCatch( url, HTTPD_MSG_PLAY,     RtspCallbackId, (void *)id );
    httpd_UrlCatch( url, HTTPD_MSG_PAUSE,    RtspCallbackId, (void *)id );
    httpd_UrlCatch( url, HTTPD_MSG_GETPARAMETER, RtspCallbackId, (void *)id );
    httpd_UrlCatch( url, HTTPD_MSG_TEARDOWN, RtspCallbackId, (void *)id );
    rtsp->track_id++;
    return id;
}
