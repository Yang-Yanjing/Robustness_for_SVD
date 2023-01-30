}
static inline bool IsIndex(intf_sys_t *sys, playlist_t *p_playlist, int i)
{
    playlist_item_t *item = sys->plist[i]->item;
    PL_ASSERT_LOCKED;
    vlc_mutex_lock(&sys->pl_lock);
    if (item->i_children == 0 && item == sys->node) {
        vlc_mutex_unlock(&sys->pl_lock);
        return true;
    }
    vlc_mutex_unlock(&sys->pl_lock);
    playlist_item_t *p_played_item = playlist_CurrentPlayingItem(p_playlist);
    if (p_played_item && item->p_input && p_played_item->p_input)
        return item->p_input->i_id == p_played_item->p_input->i_id;
    return false;
}
