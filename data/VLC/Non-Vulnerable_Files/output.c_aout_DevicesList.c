 */
int aout_DevicesList (audio_output_t *aout, char ***ids, char ***names)
{
    aout_owner_t *owner = aout_owner (aout);
    char **tabid, **tabname;
    unsigned count;
    vlc_mutex_lock (&owner->dev.lock);
    count = owner->dev.count;
    tabid = xmalloc (sizeof (*tabid) * count);
    tabname = xmalloc (sizeof (*tabname) * count);
    *ids = tabid;
    *names = tabname;
    for (aout_dev_t *dev = owner->dev.list; dev != NULL; dev = dev->next)
    {
        *(tabid++) = xstrdup (dev->id);
        *(tabname++) = xstrdup (dev->name);
    }
    vlc_mutex_unlock (&owner->dev.lock);
    return count;
}
