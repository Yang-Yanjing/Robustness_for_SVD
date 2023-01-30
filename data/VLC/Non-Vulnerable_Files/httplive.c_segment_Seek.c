}
static int segment_Seek(stream_t *s, const uint64_t pos)
{
    stream_sys_t *p_sys = s->p_sys;
    hls_stream_t *hls = hls_Get(p_sys->hls_stream, p_sys->playback.stream);
    if (hls == NULL)
        return VLC_EGENERIC;
    vlc_mutex_lock(&hls->lock);
    bool b_found = false;
    uint64_t length = 0;
    uint64_t size = hls->size;
    int count = vlc_array_count(hls->segments);
    segment_t *currentSegment = segment_GetSegment(hls, p_sys->playback.segment);
    if (currentSegment == NULL)
    {
        vlc_mutex_unlock(&hls->lock);
        return VLC_EGENERIC;
    }
    for (int n = 0; n < count; n++)
    {
        segment_t *segment = segment_GetSegment(hls, n);
        if (segment == NULL)
        {
            vlc_mutex_unlock(&hls->lock);
            return VLC_EGENERIC;
        }
        vlc_mutex_lock(&segment->lock);
        length += segment->duration * (hls->bandwidth/8);
        vlc_mutex_unlock(&segment->lock);
        if (pos <= length)
        {
            if (count - n >= 3)
            {
                p_sys->playback.segment = n;
                b_found = true;
                break;
            }
            /* Do not search in last 3 segments */
            vlc_mutex_unlock(&hls->lock);
            return VLC_EGENERIC;
        }
    }
    /* */
    if (!b_found && (pos >= size))
    {
        p_sys->playback.segment = count - 1;
        b_found = true;
    }
    /* */
    if (b_found)
    {
        /* restore current segment to start position */
        vlc_mutex_lock(&currentSegment->lock);
        segment_RestorePos(currentSegment);
        vlc_mutex_unlock(&currentSegment->lock);
        /* restore seeked segment to start position */
        segment_t *segment = segment_GetSegment(hls, p_sys->playback.segment);
        if (segment == NULL)
        {
            vlc_mutex_unlock(&hls->lock);
            return VLC_EGENERIC;
        }
        vlc_mutex_lock(&segment->lock);
        segment_RestorePos(segment);
        vlc_mutex_unlock(&segment->lock);
        /* start download at current playback segment */
        vlc_mutex_unlock(&hls->lock);
        /* Wake up download thread */
        vlc_mutex_lock(&p_sys->download.lock_wait);
        p_sys->download.seek = p_sys->playback.segment;
        vlc_cond_signal(&p_sys->download.wait);
        /* Wait for download to be finished */
        msg_Dbg(s, "seek to segment %d", p_sys->playback.segment);
        while ((p_sys->download.seek != -1) ||
           ((p_sys->download.segment - p_sys->playback.segment < 3) &&
                (p_sys->download.segment < count)))
        {
            vlc_cond_wait(&p_sys->download.wait, &p_sys->download.lock_wait);
            if (p_sys->b_error || atomic_load(&p_sys->closing))
                break;
        }
        vlc_mutex_unlock(&p_sys->download.lock_wait);
        return VLC_SUCCESS;
    }
    vlc_mutex_unlock(&hls->lock);
    return b_found ? VLC_SUCCESS : VLC_EGENERIC;
}
