}
static int parse_AddSegment(hls_stream_t *hls, const int duration, const char *uri)
{
    assert(hls);
    assert(uri);
    /* Store segment information */
    vlc_mutex_lock(&hls->lock);
    char *psz_uri = relative_URI(hls->url, uri);
    segment_t *segment = segment_New(hls, duration, psz_uri ? psz_uri : uri);
    if (segment)
        segment->sequence = hls->sequence + vlc_array_count(hls->segments) - 1;
    free(psz_uri);
    vlc_mutex_unlock(&hls->lock);
    return segment ? VLC_SUCCESS : VLC_ENOMEM;
}
