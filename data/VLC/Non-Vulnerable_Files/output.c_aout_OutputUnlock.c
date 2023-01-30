}
void aout_OutputUnlock (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    vlc_assert_locked (&owner->lock);
    vlc_mutex_lock (&owner->req.lock);
    if (owner->req.device != unset_str)
    {
        aout_OutputDeviceSet (aout, owner->req.device);
        free (owner->req.device);
        owner->req.device = (char *)unset_str;
    }
    if (owner->req.volume >= 0.f)
    {
        aout_OutputVolumeSet (aout, owner->req.volume);
        owner->req.volume = -1.f;
    }
    if (owner->req.mute >= 0)
    {
        aout_OutputMuteSet (aout, owner->req.mute);
        owner->req.mute = -1;
    }
    vlc_mutex_unlock (&owner->lock);
    /* If another thread is blocked waiting for owner->req.lock at this point,
     * this aout_OutputUnlock() call will not see and apply its change request.
     * The other thread will need to apply the change request itself, which
     * implies it is able to (try-)lock owner->lock. Therefore this thread must
     * release owner->lock _before_ owner->req.lock. Do not reorder!!! */
    vlc_mutex_unlock (&owner->req.lock);
}
