}
static int get_HTTPLiveMetaPlaylist(stream_t *s, vlc_array_t **streams)
{
    stream_sys_t *p_sys = s->p_sys;
    assert(*streams);
    int err = VLC_EGENERIC;
    /* Duplicate HLS stream META information */
    for (int i = 0; i < vlc_array_count(p_sys->hls_stream); i++)
    {
        hls_stream_t *src, *dst;
        src = hls_Get(p_sys->hls_stream, i);
        if (src == NULL)
            return VLC_EGENERIC;
        dst = hls_Copy(src, false);
        if (dst == NULL)
            return VLC_ENOMEM;
        vlc_array_append(*streams, dst);
        /* Download playlist file from server */
        uint8_t *buf = NULL;
        ssize_t len = read_M3U8_from_url(s, dst->url, &buf);
        if (len < 0)
            err = VLC_EGENERIC;
        else
        {
            /* Parse HLS m3u8 content. */
            err = parse_M3U8(s, *streams, buf, len, false);
            free(buf);
        }
    }
    return err;
}
