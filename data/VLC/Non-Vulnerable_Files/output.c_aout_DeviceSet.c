 */
int aout_DeviceSet (audio_output_t *aout, const char *id)
{
    aout_owner_t *owner = aout_owner (aout);
    char *dev = NULL;
    if (id != NULL)
    {
        dev = strdup (id);
        if (unlikely(dev == NULL))
            return -1;
    }
    vlc_mutex_lock (&owner->req.lock);
    if (owner->req.device != unset_str)
        free (owner->req.device);
    owner->req.device = dev;
    vlc_mutex_unlock (&owner->req.lock);
    if (aout_OutputTryLock (aout) == 0)
        aout_OutputUnlock (aout);
    return 0;
}
