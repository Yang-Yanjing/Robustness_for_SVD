}
static void Pause(audio_output_t *p_aout, bool pause, mtime_t date)
{
    VLC_UNUSED(date);
    aout_sys_t *p_sys = p_aout->sys;
    if (pause) {
        p_sys->at_pause(p_sys->AudioTrack);
    } else {
        p_sys->at_start(p_sys->AudioTrack);
    }
}
