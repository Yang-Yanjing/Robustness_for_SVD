 */
int aout_MuteSet (audio_output_t *aout, bool mute)
{
    aout_owner_t *owner = aout_owner (aout);
    vlc_mutex_lock (&owner->req.lock);
    owner->req.mute = mute;
    vlc_mutex_unlock (&owner->req.lock);
    if (aout_OutputTryLock (aout) == 0)
        aout_OutputUnlock (aout);
    return 0;
}
