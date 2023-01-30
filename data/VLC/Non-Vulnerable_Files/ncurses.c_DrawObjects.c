}
static int DrawObjects(intf_thread_t *intf)
{
    return SubDrawObject(intf->p_sys, 0, VLC_OBJECT(intf->p_libvlc), 0, "");
}
