}
static int parse_EndList(stream_t *s, hls_stream_t *hls)
{
    assert(hls);
    s->p_sys->b_live = false;
    msg_Dbg(s, "video on demand (vod) mode");
    return VLC_SUCCESS;
}
