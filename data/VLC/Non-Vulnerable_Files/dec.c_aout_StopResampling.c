 */
static void aout_StopResampling (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    owner->sync.resamp_type = AOUT_RESAMPLING_NONE;
    aout_FiltersAdjustResampling (owner->filters, 0);
}
