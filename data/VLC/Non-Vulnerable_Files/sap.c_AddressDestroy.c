}
static void AddressDestroy (sap_address_t *addr)
{
    assert (addr->first == NULL);
    vlc_cancel (addr->thread);
    vlc_join (addr->thread, NULL);
    vlc_cond_destroy (&addr->wait);
    vlc_mutex_destroy (&addr->lock);
    net_Close (addr->fd);
    free (addr);
}
