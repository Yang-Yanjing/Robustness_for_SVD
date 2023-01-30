}
static void Eject(intf_thread_t *intf)
{
    char *device, *name;
    playlist_t * p_playlist = pl_Get(intf);
    /* If there's a stream playing, we aren't allowed to eject ! */
    if (intf->p_sys->p_input)
        return;
    PL_LOCK;
    if (!playlist_CurrentPlayingItem(p_playlist)) {
        PL_UNLOCK;
        return;
    }
    name = playlist_CurrentPlayingItem(p_playlist)->p_input->psz_name;
    device = name ? GetDiscDevice(intf, name) : NULL;
    PL_UNLOCK;
    if (device) {
        intf_Eject(intf, device);
        free(device);
    }
}
