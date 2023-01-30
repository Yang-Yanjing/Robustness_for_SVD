}
static int aout_OutputTryLock (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    return vlc_mutex_trylock (&owner->lock);
}
