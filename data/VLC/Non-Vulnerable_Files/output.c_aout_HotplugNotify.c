}
static void aout_HotplugNotify (audio_output_t *aout,
                                const char *id, const char *name)
{
    aout_owner_t *owner = aout_owner (aout);
    aout_dev_t *dev, **pp = &owner->dev.list;
    vlc_mutex_lock (&owner->dev.lock);
    while ((dev = *pp) != NULL)
    {
        if (!strcmp (id, dev->id))
            break;
        pp = &dev->next;
    }
    if (name != NULL)
    {
        if (dev == NULL) /* Added device */
        {
            dev = malloc (sizeof (*dev) + strlen (id));
            if (unlikely(dev == NULL))
                goto out;
            dev->next = NULL;
            strcpy (dev->id, id);
            *pp = dev;
            owner->dev.count++;
        }
        else /* Modified device */
            free (dev->name);
        dev->name = strdup (name);
    }
    else
    {
        if (dev != NULL) /* Removed device */
        {
            owner->dev.count--;
            *pp = dev->next;
            free (dev->name);
            free (dev);
        }
    }
out:
    vlc_mutex_unlock (&owner->dev.lock);
}
