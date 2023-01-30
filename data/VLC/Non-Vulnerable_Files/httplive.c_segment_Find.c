}
static segment_t *segment_Find(hls_stream_t *hls, const int sequence)
{
    assert(hls);
    int count = vlc_array_count(hls->segments);
    if (count <= 0) return NULL;
    for (int n = 0; n < count; n++)
    {
        segment_t *segment = segment_GetSegment(hls, n);
        if (segment == NULL) break;
        if (segment->sequence == sequence)
            return segment;
    }
    return NULL;
}
