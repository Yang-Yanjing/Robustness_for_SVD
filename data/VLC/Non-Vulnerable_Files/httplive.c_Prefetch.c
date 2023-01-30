}
static int Prefetch(stream_t *s, int *current)
{
    stream_sys_t *p_sys = s->p_sys;
    int stream = *current;
    hls_stream_t *hls = hls_Get(p_sys->hls_stream, stream);
    if (hls == NULL)
        return VLC_EGENERIC;
    if (vlc_array_count(hls->segments) == 0)
        return VLC_EGENERIC;
    else if (vlc_array_count(hls->segments) == 1 && p_sys->b_live)
        msg_Warn(s, "Only 1 segment available to prefetch in live stream; may stall");
    /* Download ~10s worth of segments of this HLS stream if they exist */
    unsigned segment_amount = (0.5f + 10/hls->duration);
    for (unsigned i = 0; i < __MIN((unsigned)vlc_array_count(hls->segments), segment_amount); i++)
    {
        segment_t *segment = segment_GetSegment(hls, p_sys->download.segment);
        if (segment == NULL )
            return VLC_EGENERIC;
        /* It is useless to lock the segment here, as Prefetch is called before
           download and playlit thread are started. */
        if (segment->data)
        {
            p_sys->download.segment++;
            continue;
        }
        if (hls_DownloadSegmentData(s, hls, segment, current) != VLC_SUCCESS)
            return VLC_EGENERIC;
        p_sys->download.segment++;
        /* adapt bandwidth? */
        if (*current != stream)
        {
            hls_stream_t *hls = hls_Get(p_sys->hls_stream, *current);
            if (hls == NULL)
                return VLC_EGENERIC;
             stream = *current;
        }
    }
    return VLC_SUCCESS;
}
