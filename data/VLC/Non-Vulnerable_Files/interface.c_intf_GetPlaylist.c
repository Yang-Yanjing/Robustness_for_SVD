 */
static playlist_t *intf_GetPlaylist(libvlc_int_t *libvlc)
{
    playlist_t *playlist;
    vlc_mutex_lock(&lock);
    playlist = libvlc_priv(libvlc)->playlist;
    if (playlist == NULL)
    {
        playlist = playlist_Create(VLC_OBJECT(libvlc));
        libvlc_priv(libvlc)->playlist = playlist;
    }
    vlc_mutex_unlock(&lock);
    return playlist;
}
