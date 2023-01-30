}
static int parse_AllowCache(stream_t *s, hls_stream_t *hls, char *p_read)
{
    assert(hls);
    char answer[4] = "\0";
    int ret = sscanf(p_read, "#EXT-X-ALLOW-CACHE:%3s", answer);
    if (ret != 1)
    {
        msg_Err(s, "#EXT-X-ALLOW-CACHE, ignoring ...");
        return VLC_EGENERIC;
    }
    hls->b_cache = (strncmp(answer, "NO", 2) != 0);
    return VLC_SUCCESS;
}
