 *****************************************************************************/
static void *Run(void *data)
{
    intf_thread_t *intf = data;
    intf_sys_t    *sys = intf->p_sys;
    playlist_t    *p_playlist = pl_Get(intf);
    var_AddCallback(p_playlist, "intf-change", PlaylistChanged, intf);
    var_AddCallback(p_playlist, "item-change", ItemChanged, intf);
    var_AddCallback(p_playlist, "playlist-item-append", PlaylistChanged, intf);
    while (!sys->exit) {
        UpdateInput(sys, p_playlist);
        Redraw(intf);
        HandleKey(intf);
    }
    var_DelCallback(p_playlist, "intf-change", PlaylistChanged, intf);
    var_DelCallback(p_playlist, "item-change", ItemChanged, intf);
    var_DelCallback(p_playlist, "playlist-item-append", PlaylistChanged, intf);
    return NULL;
}
