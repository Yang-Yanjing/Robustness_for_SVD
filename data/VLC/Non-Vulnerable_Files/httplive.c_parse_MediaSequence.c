}
static int parse_MediaSequence(stream_t *s, hls_stream_t *hls, char *p_read)
{
    assert(hls);
    int sequence;
    int ret = sscanf(p_read, "#EXT-X-MEDIA-SEQUENCE:%d", &sequence);
    if (ret != 1)
    {
        msg_Err(s, "expected #EXT-X-MEDIA-SEQUENCE:<s>");
        return VLC_EGENERIC;
    }
    if (hls->sequence > 0)
    {
        if (s->p_sys->b_live)
        {
            hls_stream_t *last = hls_GetLast(s->p_sys->hls_stream);
            segment_t *last_segment = segment_GetSegment( last, vlc_array_count( last->segments ) - 1 );
            if ( ( last_segment->sequence < sequence) &&
                 ( sequence - last_segment->sequence >= 1 ))
                msg_Err(s, "EXT-X-MEDIA-SEQUENCE gap in playlist (new=%d, old=%d)",
                            sequence, last_segment->sequence);
        }
        else
            msg_Err(s, "EXT-X-MEDIA-SEQUENCE already present in playlist (new=%d, old=%d)",
                        sequence, hls->sequence);
    }
    hls->sequence = sequence;
    return VLC_SUCCESS;
}
