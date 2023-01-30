}
static ssize_t read_M3U8_from_url(stream_t *s, const char* psz_url, uint8_t **buffer)
{
    assert(*buffer == NULL);
    /* Construct URL */
    stream_t *p_m3u8 = stream_UrlNew(s, psz_url);
    if (p_m3u8 == NULL)
        return VLC_EGENERIC;
    ssize_t size = read_M3U8_from_stream(p_m3u8, buffer);
    stream_Delete(p_m3u8);
    return size;
}
