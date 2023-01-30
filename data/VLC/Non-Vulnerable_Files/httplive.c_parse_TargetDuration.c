}
static int parse_TargetDuration(stream_t *s, hls_stream_t *hls, char *p_read)
{
    assert(hls);
    int duration = -1;
    int ret = sscanf(p_read, "#EXT-X-TARGETDURATION:%d", &duration);
    if (ret != 1)
    {
        msg_Err(s, "expected #EXT-X-TARGETDURATION:<s>");
        return VLC_EGENERIC;
    }
    hls->duration = duration; /* seconds */
    return VLC_SUCCESS;
}
