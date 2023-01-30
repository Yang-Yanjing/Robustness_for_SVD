}
static void RtspTimeOut( void *data )
{
    rtsp_stream_t *rtsp = data;
    vlc_mutex_lock(&rtsp->lock);
    mtime_t now = mdate();
    for (int i = rtsp->sessionc - 1; i >= 0; i--)
    {
        if (rtsp->sessionv[i]->last_seen + rtsp->timeout * CLOCK_FREQ < now)
        {
            if (rtsp->vod_media != NULL)
            {
                char psz_sesbuf[17];
                snprintf( psz_sesbuf, sizeof( psz_sesbuf ), "%"PRIx64,
                          rtsp->sessionv[i]->id );
                vod_stop(rtsp->vod_media, psz_sesbuf);
            }
            RtspClientDel(rtsp, rtsp->sessionv[i]);
        }
    }
    RtspUpdateTimer(rtsp);
    vlc_mutex_unlock(&rtsp->lock);
}
