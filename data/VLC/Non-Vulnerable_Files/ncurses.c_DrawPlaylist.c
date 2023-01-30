}
static int DrawPlaylist(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    playlist_t *p_playlist = pl_Get(intf);
    PL_LOCK;
    vlc_mutex_lock(&sys->pl_lock);
    if (sys->need_update) {
        PlaylistRebuild(intf);
        sys->need_update = false;
    }
    vlc_mutex_unlock(&sys->pl_lock);
    PL_UNLOCK;
    if (sys->plidx_follow)
        FindIndex(sys, p_playlist);
    for (int i = 0; i < sys->plist_entries; i++) {
        char c;
        playlist_item_t *current_item;
        playlist_item_t *item = sys->plist[i]->item;
        vlc_mutex_lock(&sys->pl_lock);
        playlist_item_t *node = sys->node;
        vlc_mutex_unlock(&sys->pl_lock);
        PL_LOCK;
        assert(item);
        current_item = playlist_CurrentPlayingItem(p_playlist);
        if ((node && item->p_input == node->p_input) ||
           (!node && current_item && item->p_input == current_item->p_input))
            c = '*';
        else if (item == node || current_item == item)
            c = '>';
        else
            c = ' ';
        PL_UNLOCK;
        if (sys->color) color_set(i%3 + C_PLAYLIST_1, NULL);
        MainBoxWrite(sys, i, "%c%s", c, sys->plist[i]->display);
        if (sys->color) color_set(C_DEFAULT, NULL);
    }
    return sys->plist_entries;
}
