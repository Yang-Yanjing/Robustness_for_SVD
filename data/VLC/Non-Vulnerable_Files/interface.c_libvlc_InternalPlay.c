}
void libvlc_InternalPlay(libvlc_int_t *libvlc)
{
    playlist_t *pl;
    vlc_mutex_lock(&lock);
    pl = libvlc_priv(libvlc)->playlist;
    vlc_mutex_unlock(&lock);
    if (pl != NULL && var_GetBool(pl, "playlist-autostart"))
        playlist_Control(pl, PLAYLIST_PLAY, false);
}
