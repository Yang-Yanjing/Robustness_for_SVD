}
static int parse_Version(stream_t *s, hls_stream_t *hls, char *p_read)
{
    assert(hls);
    int version;
    int ret = sscanf(p_read, "#EXT-X-VERSION:%d", &version);
    if (ret != 1)
    {
        msg_Err(s, "#EXT-X-VERSION: no protocol version found, should be version 1.");
        return VLC_EGENERIC;
    }
    /* Check version */
    hls->version = version;
    if (hls->version <= 0 || hls->version > 3)
    {
        msg_Err(s, "#EXT-X-VERSION should be version 1, 2 or 3 iso %d", version);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
