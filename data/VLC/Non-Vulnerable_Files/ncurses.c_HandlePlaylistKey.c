}
static bool HandlePlaylistKey(intf_thread_t *intf, int key)
{
    intf_sys_t *sys = intf->p_sys;
    playlist_t *p_playlist = pl_Get(intf);
    struct pl_item_t *p_pl_item;
    switch(key)
    {
    /* Playlist Settings */
    case 'r': var_ToggleBool(p_playlist, "random"); return true;
    case 'l': var_ToggleBool(p_playlist, "loop");   return true;
    case 'R': var_ToggleBool(p_playlist, "repeat"); return true;
    /* Playlist sort */
    case 'o':
    case 'O':
        playlist_RecursiveNodeSort(p_playlist, p_playlist->p_root_onelevel,
                                    SORT_TITLE_NODES_FIRST,
                                    (key == 'o')? ORDER_NORMAL : ORDER_REVERSE);
        vlc_mutex_lock(&sys->pl_lock);
        sys->need_update = true;
        vlc_mutex_unlock(&sys->pl_lock);
        return true;
    case ';':
        SearchPlaylist(sys);
        return true;
    case 'g':
        FindIndex(sys, p_playlist);
        return true;
    /* Deletion */
    case 'D':
    case KEY_BACKSPACE:
    case 0x7f:
    case KEY_DC:
    {
        playlist_item_t *item;
        PL_LOCK;
        item = sys->plist[sys->box_idx]->item;
        if (item->i_children == -1)
            playlist_DeleteFromInput(p_playlist, item->p_input, pl_Locked);
        else
            playlist_NodeDelete(p_playlist, item, true , false);
        PL_UNLOCK;
        vlc_mutex_lock(&sys->pl_lock);
        if (sys->box_idx >= sys->box_lines_total - 1)
            sys->box_idx = sys->box_lines_total - 2;
        sys->need_update = true;
        vlc_mutex_unlock(&sys->pl_lock);
        return true;
    }
    case KEY_ENTER:
    case '\r':
    case '\n':
        if (!(p_pl_item = sys->plist[sys->box_idx]))
            return false;
        if (p_pl_item->item->i_children) {
            playlist_item_t *item, *p_parent = p_pl_item->item;
            if (p_parent->i_children == -1) {
                item = p_parent;
                while (p_parent->p_parent)
                    p_parent = p_parent->p_parent;
            } else {
                vlc_mutex_lock(&sys->pl_lock);
                sys->node = p_parent;
                vlc_mutex_unlock(&sys->pl_lock);
                item = NULL;
            }
            playlist_Control(p_playlist, PLAYLIST_VIEWPLAY, pl_Unlocked,
                              p_parent, item);
        } else {   /* We only want to set the current node */
            playlist_Stop(p_playlist);
            vlc_mutex_lock(&sys->pl_lock);
            sys->node = p_pl_item->item;
            vlc_mutex_unlock(&sys->pl_lock);
        }
        sys->plidx_follow = true;
        return true;
    }
    return false;
}
