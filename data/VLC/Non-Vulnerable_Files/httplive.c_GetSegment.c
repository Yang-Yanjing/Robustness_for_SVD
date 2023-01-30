 ****************************************************************************/
static segment_t *GetSegment(stream_t *s)
{
    stream_sys_t *p_sys = s->p_sys;
    segment_t *segment = NULL;
    /* Is this segment of the current HLS stream ready? */
    hls_stream_t *hls = hls_Get(p_sys->hls_stream, p_sys->playback.stream);
    if (hls != NULL)
    {
        vlc_mutex_lock(&hls->lock);
        segment = segment_GetSegment(hls, p_sys->playback.segment);
        if (segment != NULL)
        {
            vlc_mutex_lock(&segment->lock);
            /* This segment is ready? */
            if (segment->data != NULL)
            {
                vlc_mutex_unlock(&segment->lock);
                p_sys->b_cache = hls->b_cache;
                vlc_mutex_unlock(&hls->lock);
                goto check;
            }
            vlc_mutex_unlock(&segment->lock);
        }
        vlc_mutex_unlock(&hls->lock);
    }
    /* Was the HLS stream changed to another bitrate? */
    segment = NULL;
    for (int i_stream = 0; i_stream < vlc_array_count(p_sys->hls_stream); i_stream++)
    {
        /* Is the next segment ready */
        hls_stream_t *hls = hls_Get(p_sys->hls_stream, i_stream);
        if (hls == NULL)
            return NULL;
        vlc_mutex_lock(&hls->lock);
        segment = segment_GetSegment(hls, p_sys->playback.segment);
        if (segment == NULL)
        {
            vlc_mutex_unlock(&hls->lock);
            break;
        }
        vlc_mutex_lock(&p_sys->download.lock_wait);
        int i_segment = p_sys->download.segment;
        vlc_mutex_unlock(&p_sys->download.lock_wait);
        vlc_mutex_lock(&segment->lock);
        /* This segment is ready? */
        if ((segment->data != NULL) &&
            (p_sys->playback.segment < i_segment))
        {
            p_sys->playback.stream = i_stream;
            p_sys->b_cache = hls->b_cache;
            vlc_mutex_unlock(&segment->lock);
            vlc_mutex_unlock(&hls->lock);
            goto check;
        }
        vlc_mutex_unlock(&segment->lock);
        vlc_mutex_unlock(&hls->lock);
        if (!p_sys->b_meta)
            break;
    }
    /* */
    return NULL;
check:
    /* sanity check */
    assert(segment->data);
    if (segment->data->i_buffer == 0)
    {
        vlc_mutex_lock(&hls->lock);
        int count = vlc_array_count(hls->segments);
        vlc_mutex_unlock(&hls->lock);
        if ((p_sys->download.segment - p_sys->playback.segment == 0) &&
            ((count != p_sys->download.segment) || p_sys->b_live))
            msg_Err(s, "playback will stall");
        else if ((p_sys->download.segment - p_sys->playback.segment < 3) &&
                 ((count != p_sys->download.segment) || p_sys->b_live))
            msg_Warn(s, "playback in danger of stalling");
    }
    return segment;
}
