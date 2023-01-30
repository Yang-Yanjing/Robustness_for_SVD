}
static int aout_GainNotify (audio_output_t *aout, float gain)
{
    aout_owner_t *owner = aout_owner (aout);
    aout_OutputAssertLocked (aout);
    aout_volume_SetVolume (owner->volume, gain);
    /* XXX: ideally, return -1 if format cannot be amplified */
    return 0;
}
