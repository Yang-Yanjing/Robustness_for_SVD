}
static void Flush(audio_output_t *aout, bool drain)
{
    aout_sys_t *sys = aout->sys;
    if (drain) {
        mtime_t delay;
        if (!TimeGet(aout, &delay))
            msleep(delay);
    } else {
        vlc_mutex_lock(&sys->lock);
        SetPlayState(sys->playerPlay, SL_PLAYSTATE_STOPPED);
        Clear(sys->playerBufferQueue);
        SetPlayState(sys->playerPlay, SL_PLAYSTATE_PLAYING);
        /* release audio data not yet written to opensles */
        block_ChainRelease(sys->p_buffer_chain);
        sys->p_buffer_chain = NULL;
        sys->pp_buffer_last = &sys->p_buffer_chain;
        sys->samples = 0;
        sys->started = false;
        vlc_mutex_unlock(&sys->lock);
    }
}
