}
static int parse_ProgramDateTime(stream_t *s, hls_stream_t *hls, char *p_read)
{
    VLC_UNUSED(hls);
    msg_Dbg(s, "tag not supported: #EXT-X-PROGRAM-DATE-TIME %s", p_read);
    return VLC_SUCCESS;
}
