}
static void PlayedCallback (SLAndroidSimpleBufferQueueItf caller, void *pContext)
{
    (void)caller;
    audio_output_t *aout = pContext;
    aout_sys_t *sys = aout->sys;
    assert (caller == sys->playerBufferQueue);
    vlc_mutex_lock(&sys->lock);
    sys->started = true;
    vlc_mutex_unlock(&sys->lock);
}
