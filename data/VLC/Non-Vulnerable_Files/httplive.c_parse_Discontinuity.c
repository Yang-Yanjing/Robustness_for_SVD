}
static int parse_Discontinuity(stream_t *s, hls_stream_t *hls, char *p_read)
{
    assert(hls);
    /* FIXME: Do we need to act on discontinuity ?? */
    msg_Dbg(s, "#EXT-X-DISCONTINUITY %s", p_read);
    return VLC_SUCCESS;
}
