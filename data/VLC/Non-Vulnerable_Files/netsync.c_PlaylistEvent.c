}
static int PlaylistEvent(vlc_object_t *object, char const *cmd,
                         vlc_value_t oldval, vlc_value_t newval, void *data)
{
    VLC_UNUSED(cmd); VLC_UNUSED(oldval); VLC_UNUSED(object);
    intf_thread_t  *intf = data;
    intf_sys_t     *sys = intf->p_sys;
    input_thread_t *input = newval.p_address;
    assert(sys->input == NULL);
    sys->input = vlc_object_hold(input);
    if (vlc_clone(&sys->thread, sys->is_master ? Master : Slave, intf,
                  VLC_THREAD_PRIORITY_INPUT)) {
        vlc_object_release(input);
        sys->input = NULL;
        return VLC_SUCCESS;
    }
    var_AddCallback(input, "intf-event", InputEvent, intf);
    return VLC_SUCCESS;
}
