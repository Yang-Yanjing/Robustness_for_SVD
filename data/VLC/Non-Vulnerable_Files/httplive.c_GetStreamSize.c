}
static uint64_t GetStreamSize(stream_t *s)
{
    stream_sys_t *p_sys = s->p_sys;
    if (p_sys->b_live)
        return 0;
    hls_stream_t *hls = hls_Get(p_sys->hls_stream, p_sys->playback.stream);
    if (hls == NULL) return 0;
    vlc_mutex_lock(&hls->lock);
    if (hls->size == 0)
        hls->size = hls_GetStreamSize(hls);
    uint64_t size = hls->size;
    vlc_mutex_unlock(&hls->lock);
    return size;
}
