 *****************************************************************************/
void Close(vlc_object_t *object)
{
    intf_thread_t *intf = (intf_thread_t*)object;
    intf_sys_t *sys = intf->p_sys;
    assert(sys->input == NULL);
    var_DelCallback(sys->playlist, "input-current", PlaylistEvent, intf);
    net_Close(sys->fd);
    free(sys);
}
