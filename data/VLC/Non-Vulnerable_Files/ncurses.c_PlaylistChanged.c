}
static int PlaylistChanged(vlc_object_t *p_this, const char *variable,
                            vlc_value_t oval, vlc_value_t nval, void *param)
{
    VLC_UNUSED(p_this); VLC_UNUSED(variable);
    VLC_UNUSED(oval); VLC_UNUSED(nval);
    intf_thread_t *intf   = (intf_thread_t *)param;
    intf_sys_t *sys       = intf->p_sys;
    playlist_item_t *node = playlist_CurrentPlayingItem(pl_Get(intf));
    vlc_mutex_lock(&sys->pl_lock);
    sys->need_update = true;
    sys->node = node ? node->p_parent : NULL;
    vlc_mutex_unlock(&sys->pl_lock);
    return VLC_SUCCESS;
}
