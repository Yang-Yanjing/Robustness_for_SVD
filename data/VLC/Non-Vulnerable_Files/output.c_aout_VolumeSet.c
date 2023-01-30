 */
int aout_VolumeSet (audio_output_t *aout, float vol)
{
    aout_owner_t *owner = aout_owner (aout);
    assert (vol >= 0.f);
    vlc_mutex_lock (&owner->req.lock);
    owner->req.volume = vol;
    vlc_mutex_unlock (&owner->req.lock);
    if (aout_OutputTryLock (aout) == 0)
        aout_OutputUnlock (aout);
    return 0;
}
