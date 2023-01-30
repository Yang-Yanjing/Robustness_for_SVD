}
static void Pause (audio_output_t *p_aout, bool pause, mtime_t date)
{
    struct aout_sys_t * p_sys = p_aout->sys;
    VLC_UNUSED(date);
    vlc_mutex_lock(&p_sys->lock);
    p_sys->b_paused = pause;
    vlc_mutex_unlock(&p_sys->lock);
    /* we need to start / stop the audio unit here because otherwise
     * the OS won't believe us that we stopped the audio output
     * so in case of an interruption, our unit would be permanently
     * silenced.
     * in case of multi-tasking, the multi-tasking view would still
     * show a playing state despite we are paused, same for lock screen */
    if (pause) {
        AudioOutputUnitStop(p_sys->au_unit);
        AudioSessionSetActive(false);
    } else {
        UInt32 sessionCategory = kAudioSessionCategory_MediaPlayback;
        AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(sessionCategory),&sessionCategory);
        AudioSessionSetActive(true);
        AudioOutputUnitStart(p_sys->au_unit);
    }
}
