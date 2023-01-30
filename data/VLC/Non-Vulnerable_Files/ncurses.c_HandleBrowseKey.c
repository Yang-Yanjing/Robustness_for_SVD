}
static bool HandleBrowseKey(intf_thread_t *intf, int key)
{
    intf_sys_t *sys = intf->p_sys;
    struct dir_entry_t *dir_entry;
    switch(key)
    {
    case '.':
        sys->show_hidden_files = !sys->show_hidden_files;
        ReadDir(intf);
        return true;
    case KEY_ENTER:
    case '\r':
    case '\n':
    case ' ':
        dir_entry = sys->dir_entries[sys->box_idx];
        char *path;
        if (asprintf(&path, "%s" DIR_SEP "%s", sys->current_dir,
                     dir_entry->path) == -1)
            return true;
        if (!dir_entry->file && key != ' ') {
            free(sys->current_dir);
            sys->current_dir = path;
            ReadDir(intf);
            sys->box_start = 0;
            sys->box_idx = 0;
            return true;
        }
        char *uri = vlc_path2uri(path, "file");
        free(path);
        if (uri == NULL)
            return true;
        playlist_t *p_playlist = pl_Get(intf);
        vlc_mutex_lock(&sys->pl_lock);
        playlist_item_t *p_parent = sys->node;
        vlc_mutex_unlock(&sys->pl_lock);
        if (!p_parent) {
            playlist_item_t *item;
            PL_LOCK;
            item = playlist_CurrentPlayingItem(p_playlist);
            p_parent = item ? item->p_parent : NULL;
            PL_UNLOCK;
            if (!p_parent)
                p_parent = p_playlist->p_local_onelevel;
        }
        while (p_parent->p_parent && p_parent->p_parent->p_parent)
            p_parent = p_parent->p_parent;
        input_item_t *p_input = p_playlist->p_local_onelevel->p_input;
        playlist_Add(p_playlist, uri, NULL, PLAYLIST_APPEND,
                      PLAYLIST_END, p_parent->p_input == p_input, false);
        BoxSwitch(sys, BOX_PLAYLIST);
        free(uri);
        return true;
    }
    return false;
}
