}
static bool hls_MaySeek(stream_t *s)
{
    stream_sys_t *p_sys = s->p_sys;
    if (p_sys->hls_stream == NULL)
        return false;
    hls_stream_t *hls = hls_Get(p_sys->hls_stream, p_sys->playback.stream);
    if (hls == NULL) return false;
    if (p_sys->b_live)
    {
        vlc_mutex_lock(&hls->lock);
        int count = vlc_array_count(hls->segments);
        vlc_mutex_unlock(&hls->lock);
        vlc_mutex_lock(&p_sys->download.lock_wait);
        bool may_seek = (p_sys->download.segment < (count - 2));
        vlc_mutex_unlock(&p_sys->download.lock_wait);
        return may_seek;
    }
    return true;
}
