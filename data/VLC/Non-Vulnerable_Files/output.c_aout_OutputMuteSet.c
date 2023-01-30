}
static int aout_OutputMuteSet (audio_output_t *aout, bool mute)
{
    aout_OutputAssertLocked (aout);
    return (aout->mute_set != NULL) ? aout->mute_set (aout, mute) : -1;
}
