}
void aout_OutputLock (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    vlc_mutex_lock (&owner->lock);
}
