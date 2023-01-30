}
static int MuteSet (audio_output_t *aout, bool mute)
{
    aout_sys_t *sys = aout->sys;
    sys->mute = mute;
    if (!sys->ready)
        return 0; /* sys->opaque is not yet defined... */
    return sys->set_volume (sys->opaque, sys->volume, mute) ? -1 : 0;
}
