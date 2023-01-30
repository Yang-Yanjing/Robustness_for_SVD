}
static void VolumeChanged (void *arg, unsigned volume)
{
    audio_output_t *aout = arg;
    float fvol = (float)volume / (float)SIO_MAXVOL;
    aout_VolumeReport (aout, fvol);
    aout_MuteReport (aout, volume == 0);
    if (volume) /* remember last non-zero volume to unmute later */
        aout->sys->volume = volume;
}
