}
static int parse_StreamInformation(stream_t *s, vlc_array_t **hls_stream,
                                   hls_stream_t **hls, char *p_read, const char *uri)
{
    int id;
    uint64_t bw;
    char *attr;
    assert(*hls == NULL);
    attr = parse_Attributes(p_read, "PROGRAM-ID");
    if (attr)
    {
        id = atol(attr);
        free(attr);
    }
    else
        id = 0;
    attr = parse_Attributes(p_read, "BANDWIDTH");
    if (attr == NULL)
    {
        msg_Err(s, "#EXT-X-STREAM-INF: expected BANDWIDTH=<value>");
        return VLC_EGENERIC;
    }
    bw = atoll(attr);
    free(attr);
    if (bw == 0)
    {
        msg_Err(s, "#EXT-X-STREAM-INF: bandwidth cannot be 0");
        return VLC_EGENERIC;
    }
    msg_Dbg(s, "bandwidth adaptation detected (program-id=%d, bandwidth=%"PRIu64").", id, bw);
    char *psz_uri = relative_URI(s->p_sys->m3u8, uri);
    *hls = hls_New(*hls_stream, id, bw, psz_uri ? psz_uri : uri);
    free(psz_uri);
    return (*hls == NULL) ? VLC_ENOMEM : VLC_SUCCESS;
}
