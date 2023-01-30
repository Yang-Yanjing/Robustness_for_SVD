 */
static void aout_Destructor (vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_owner_t *owner = aout_owner (aout);
    vlc_mutex_destroy (&owner->dev.lock);
    for (aout_dev_t *dev = owner->dev.list, *next; dev != NULL; dev = next)
    {
        next = dev->next;
        free (dev->name);
        free (dev);
    }
    assert (owner->req.device == unset_str);
    vlc_mutex_destroy (&owner->req.lock);
    vlc_mutex_destroy (&owner->lock);
}
