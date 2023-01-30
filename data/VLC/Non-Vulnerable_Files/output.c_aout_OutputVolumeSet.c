}
static int aout_OutputVolumeSet (audio_output_t *aout, float vol)
{
    aout_OutputAssertLocked (aout);
    return (aout->volume_set != NULL) ? aout->volume_set (aout, vol) : -1;
}
