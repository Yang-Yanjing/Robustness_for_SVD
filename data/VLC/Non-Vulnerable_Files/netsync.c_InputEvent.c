}
static int InputEvent(vlc_object_t *object, char const *cmd,
                      vlc_value_t oldval, vlc_value_t newval, void *data)
{
    VLC_UNUSED(cmd); VLC_UNUSED(oldval); VLC_UNUSED(object);
    intf_thread_t  *intf = data;
    intf_sys_t     *sys = intf->p_sys;
    if (newval.i_int == INPUT_EVENT_DEAD && sys->input) {
        msg_Err(intf, "InputEvent DEAD");
        vlc_cancel(sys->thread);
        vlc_join(sys->thread, NULL);
        vlc_object_release(sys->input);
        sys->input = NULL;
    }
    return VLC_SUCCESS;
}
