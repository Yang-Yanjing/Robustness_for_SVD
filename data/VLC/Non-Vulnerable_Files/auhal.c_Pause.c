}
static void Pause(audio_output_t *p_aout, bool pause, mtime_t date)
{
    struct aout_sys_t * p_sys = p_aout->sys;
    VLC_UNUSED(date);
    vlc_mutex_lock(&p_sys->lock);
    p_sys->b_paused = pause;
    vlc_mutex_unlock(&p_sys->lock);
}
