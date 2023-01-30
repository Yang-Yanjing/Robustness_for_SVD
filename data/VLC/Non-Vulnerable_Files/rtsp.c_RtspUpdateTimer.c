/** rtsp must be locked */
static void RtspUpdateTimer( rtsp_stream_t *rtsp )
{
    if (rtsp->timeout <= 0)
        return;
    mtime_t timeout = 0;
    for (int i = 0; i < rtsp->sessionc; i++)
    {
        if (timeout == 0 || rtsp->sessionv[i]->last_seen < timeout)
            timeout = rtsp->sessionv[i]->last_seen;
    }
    if (timeout != 0)
        timeout += rtsp->timeout * CLOCK_FREQ;
    vlc_timer_schedule(rtsp->timer, true, timeout, 0);
}
