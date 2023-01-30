 *****************************************************************************/
static int Open(vlc_object_t *object)
{
    intf_thread_t *intf = (intf_thread_t*)object;
    intf_sys_t    *sys;
    int fd;
    if (!var_InheritBool(intf, "netsync-master")) {
        char *psz_master = var_InheritString(intf, "netsync-master-ip");
        if (psz_master == NULL) {
            msg_Err(intf, "master address not specified");
            return VLC_EGENERIC;
        }
        fd = net_ConnectUDP(VLC_OBJECT(intf), psz_master, NETSYNC_PORT, -1);
        free(psz_master);
    } else {
        fd = net_ListenUDP1(VLC_OBJECT(intf), NULL, NETSYNC_PORT);
    }
    if (fd == -1) {
        msg_Err(intf, "Netsync socket failure");
        return VLC_EGENERIC;
    }
    intf->p_sys = sys = malloc(sizeof(*sys));
    if (!sys) {
        net_Close(fd);
        return VLC_ENOMEM;
    }
    sys->fd = fd;
    sys->is_master = var_InheritBool(intf, "netsync-master");
    sys->timeout = var_InheritInteger(intf, "netsync-timeout");
    if (sys->timeout < 500)
        sys->timeout = 500;
    sys->playlist = pl_Get(intf);
    sys->input = NULL;
    var_AddCallback(sys->playlist, "input-current", PlaylistEvent, intf);
    return VLC_SUCCESS;
}
