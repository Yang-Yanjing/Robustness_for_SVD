}
static int ChooseSegment(stream_t *s, const int current)
{
    stream_sys_t *p_sys = (stream_sys_t *)s->p_sys;
    hls_stream_t *hls = hls_Get(p_sys->hls_stream, current);
    if (hls == NULL) return 0;
    /* Choose a segment to start which is no closer than
     * 3 times the target duration from the end of the playlist.
     */
    int wanted = 0;
    int duration = 0;
    int sequence = 0;
    int count = vlc_array_count(hls->segments);
    int i = p_sys->b_live ? count - 1 : -1;
    /* We do while loop only with live case, otherwise return 0*/
    while((i >= 0) && (i < count))
    {
        segment_t *segment = segment_GetSegment(hls, i);
        assert(segment);
        if (segment->duration > hls->duration)
        {
            msg_Err(s, "EXTINF:%d duration is larger than EXT-X-TARGETDURATION:%d",
                    segment->duration, hls->duration);
        }
        duration += segment->duration;
        if (duration >= 3 * hls->duration)
        {
            /* Start point found */
            wanted = i;
            sequence = segment->sequence;
            break;
        }
        i-- ;
    }
    msg_Dbg(s, "Choose segment %d/%d (sequence=%d)", wanted, count, sequence);
    return wanted;
}
