 */
static int AddSource (services_discovery_t *sd, const pa_source_info *info)
{
    services_discovery_sys_t *sys = sd->p_sys;
    msg_Dbg (sd, "adding %s (%s)", info->name, info->description);
    char *mrl;
    if (unlikely(asprintf (&mrl, "pulse://%s", info->name) == -1))
        return -1;
    input_item_t *item = input_item_NewWithType (mrl, info->description,
                                                 0, NULL, 0, -1,
                                                 ITEM_TYPE_CARD);
    free (mrl);
    if (unlikely(item == NULL))
        return -1;
    struct device *d = malloc (sizeof (*d));
    if (unlikely(d == NULL))
    {
        vlc_gc_decref (item);
        return -1;
    }
    d->index = info->index;
    d->item = item;
    struct device **dp = tsearch (d, &sys->root, cmpsrc);
    if (dp == NULL) /* Out-of-memory */
    {
        free (d);
        vlc_gc_decref (item);
        return -1;
    }
    if (*dp != d) /* Update existing source */
    {
        free (d);
        d = *dp;
        input_item_SetURI (d->item, item->psz_uri);
        input_item_SetName (d->item, item->psz_name);
        vlc_gc_decref (item);
        return 0;
    }
    const char *card = pa_proplist_gets(info->proplist, "device.product.name");
    services_discovery_AddItem (sd, item,
                                (card != NULL) ? card : N_("Generic"));
    d->sd = sd;
    return 0;
}
