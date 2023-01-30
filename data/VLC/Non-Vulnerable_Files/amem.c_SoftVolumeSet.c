}
static int SoftVolumeSet (audio_output_t *aout, float vol)
{
    aout_sys_t *sys = aout->sys;
    vol = vol * vol * vol;
    if (!sys->mute && aout_GainRequest (aout, vol))
        return -1;
    sys->volume = vol;
    return 0;
}
