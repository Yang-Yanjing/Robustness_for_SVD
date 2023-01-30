}
static int MuteSet (audio_output_t *aout, bool mute)
{
    aout_sys_t *sys = aout->sys;
    if (!sio_setvol (sys->hdl, mute ? 0 : sys->volume))
        return -1;
    sys->mute = mute;
    return 0;
}
