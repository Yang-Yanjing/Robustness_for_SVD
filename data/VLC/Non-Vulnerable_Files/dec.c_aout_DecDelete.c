 */
void aout_DecDelete (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    aout_OutputLock (aout);
    if (owner->mixer_format.i_format)
    {
        aout_FiltersDelete (aout, owner->filters);
        aout_OutputDelete (aout);
    }
    aout_volume_Delete (owner->volume);
    owner->volume = NULL;
    aout_OutputUnlock (aout);
    var_Destroy (aout, "stereo-mode");
}
