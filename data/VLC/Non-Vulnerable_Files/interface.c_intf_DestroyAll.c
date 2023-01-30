 */
void intf_DestroyAll(libvlc_int_t *libvlc)
{
    playlist_t *playlist;
    vlc_mutex_lock(&lock);
    playlist = libvlc_priv(libvlc)->playlist;
    if (playlist != NULL)
    {
        intf_thread_t *intf, **pp = &(pl_priv(playlist)->interface);
        while ((intf = *pp) != NULL)
        {
            *pp = intf->p_next;
            vlc_mutex_unlock(&lock);
            module_unneed(intf, intf->p_module);
            config_ChainDestroy(intf->p_cfg);
            var_DelCallback(intf, "intf-add", AddIntfCallback, playlist);
            vlc_object_release(intf);
            vlc_mutex_lock(&lock);
        }
        libvlc_priv(libvlc)->playlist = NULL;
    }
    vlc_mutex_unlock(&lock);
    if (playlist != NULL)
        playlist_Destroy(playlist);
}
