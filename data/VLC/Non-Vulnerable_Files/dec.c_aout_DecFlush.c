}
void aout_DecFlush (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    aout_OutputLock (aout);
    owner->sync.end = VLC_TS_INVALID;
    if (owner->mixer_format.i_format)
        aout_OutputFlush (aout, false);
    aout_OutputUnlock (aout);
}
