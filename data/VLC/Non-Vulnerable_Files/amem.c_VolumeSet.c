}
static int VolumeSet (audio_output_t *aout, float vol)
{
    aout_sys_t *sys = aout->sys;
    sys->volume = vol;
    if (sys->ready)
        return 0; /* sys->opaque is not yet defined... */
    return sys->set_volume (sys->opaque, vol, sys->mute) ? -1 : 0;
}
