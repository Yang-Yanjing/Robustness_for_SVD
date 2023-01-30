}
static void Flush(audio_output_t *p_aout, bool wait)
{
    struct aout_sys_t *p_sys = p_aout->sys;
    int32_t availableBytes;
    vlc_mutex_lock(&p_sys->lock);
    TPCircularBufferTail(&p_sys->circular_buffer, &availableBytes);
    if (wait) {
        while (availableBytes > 0) {
            vlc_cond_wait(&p_sys->cond, &p_sys->lock);
            TPCircularBufferTail(&p_sys->circular_buffer, &availableBytes);
        }
    } else {
        /* flush circular buffer if data is left */
        if (availableBytes > 0)
            TPCircularBufferClear(&p_aout->sys->circular_buffer);
    }
    vlc_mutex_unlock(&p_sys->lock);
}
