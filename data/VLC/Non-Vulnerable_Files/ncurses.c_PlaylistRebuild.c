}
static void PlaylistRebuild(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    playlist_t *p_playlist = pl_Get(intf);
    PlaylistDestroy(sys);
    PlaylistAddNode(sys, p_playlist->p_root_onelevel, "");
}
