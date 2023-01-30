}
static hls_stream_t *hls_GetLast(vlc_array_t *hls_stream)
{
    int count = vlc_array_count(hls_stream);
    if (count <= 0)
        return NULL;
    count--;
    return hls_Get(hls_stream, count);
}
