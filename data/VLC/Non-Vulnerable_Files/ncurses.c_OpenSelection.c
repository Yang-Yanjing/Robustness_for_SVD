}
static void OpenSelection(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    char *uri = vlc_path2uri(sys->open_chain, NULL);
    if (uri == NULL)
        return;
    playlist_t *p_playlist = pl_Get(intf);
    vlc_mutex_lock(&sys->pl_lock);
    playlist_item_t *p_parent = sys->node;
    vlc_mutex_unlock(&sys->pl_lock);
    PL_LOCK;
    if (!p_parent) {
        playlist_item_t *current;
        current= playlist_CurrentPlayingItem(p_playlist);
        p_parent = current ? current->p_parent : NULL;
        if (!p_parent)
            p_parent = p_playlist->p_local_onelevel;
    }
    while (p_parent->p_parent && p_parent->p_parent->p_parent)
        p_parent = p_parent->p_parent;
    PL_UNLOCK;
    playlist_Add(p_playlist, uri, NULL,
            PLAYLIST_APPEND|PLAYLIST_GO, PLAYLIST_END,
            p_parent->p_input == p_playlist->p_local_onelevel->p_input,
            false);
    sys->plidx_follow = true;
    free(uri);
}
