}
static segment_t *segment_GetSegment(hls_stream_t *hls, const int wanted)
{
    assert(hls);
    int count = vlc_array_count(hls->segments);
    if (count <= 0)
        return NULL;
    if ((wanted < 0) || (wanted >= count))
        return NULL;
    return (segment_t *) vlc_array_item_at_index(hls->segments, wanted);
}
