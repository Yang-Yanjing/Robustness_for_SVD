 ****************************************************************************/
static void Close(vlc_object_t *p_this)
{
    stream_t *s = (stream_t*)p_this;
    stream_sys_t *p_sys = s->p_sys;
    assert(p_sys->hls_stream);
    vlc_mutex_lock(&p_sys->lock);
    p_sys->paused = false;
    atomic_store(&p_sys->closing, true);
    vlc_cond_signal(&p_sys->wait);
    vlc_mutex_unlock(&p_sys->lock);
    /* */
    vlc_mutex_lock(&p_sys->download.lock_wait);
    /* negate the condition variable's predicate */
    p_sys->download.segment = p_sys->playback.segment = 0;
    p_sys->download.seek = 0; /* better safe than sorry */
    vlc_cond_signal(&p_sys->download.wait);
    vlc_mutex_unlock(&p_sys->download.lock_wait);
    vlc_cond_signal(&p_sys->read.wait); /* set closing first */
    /* */
    if (p_sys->b_live)
    {
        vlc_cancel(p_sys->reload);
        vlc_join(p_sys->reload, NULL);
    }
    vlc_cancel(p_sys->thread);
    vlc_join(p_sys->thread, NULL);
    vlc_mutex_destroy(&p_sys->download.lock_wait);
    vlc_cond_destroy(&p_sys->download.wait);
    vlc_mutex_destroy(&p_sys->read.lock_wait);
    vlc_cond_destroy(&p_sys->read.wait);
    /* Free hls streams */
    for (int i = 0; i < vlc_array_count(p_sys->hls_stream); i++)
    {
        hls_stream_t *hls = hls_Get(p_sys->hls_stream, i);
        if (hls) hls_Free(hls);
    }
    vlc_array_destroy(p_sys->hls_stream);
    /* */
    vlc_mutex_destroy(&p_sys->lock);
    vlc_cond_destroy(&p_sys->wait);
    free(p_sys->m3u8);
    if (p_sys->peeked)
        block_Release (p_sys->peeked);
    free(p_sys);
}
