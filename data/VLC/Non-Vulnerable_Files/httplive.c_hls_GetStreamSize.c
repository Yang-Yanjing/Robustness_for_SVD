}
static uint64_t hls_GetStreamSize(hls_stream_t *hls)
{
    /* NOTE: Stream size is calculated based on segment duration and
     * HLS stream bandwidth from the .m3u8 file. If these are not correct
     * then the deviation from exact byte size will be big and the seek/
     * progressbar will not behave entirely as one expects. */
    uint64_t size = 0UL;
    /* If there is no valid bandwidth yet, then there is no point in
     * computing stream size. */
    if (hls->bandwidth == 0)
        return size;
    int count = vlc_array_count(hls->segments);
    for (int n = 0; n < count; n++)
    {
        segment_t *segment = segment_GetSegment(hls, n);
        if (segment)
        {
            size += (segment->duration * (hls->bandwidth / 8));
        }
    }
    return size;
}
