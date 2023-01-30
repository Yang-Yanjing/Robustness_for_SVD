};
static void DestroySource (void *data)
{
    struct device *d = data;
    services_discovery_RemoveItem (d->sd, d->item);
    vlc_gc_decref (d->item);
    free (d);
}
