}
static int SoftMuteSet (audio_output_t *aout, bool mute)
{
    aout_sys_t *sys = aout->sys;
    if (aout_GainRequest (aout, mute ? 0.f : sys->volume))
        return -1;
    sys->mute = mute;
    return 0;
}
