static int
MarshalDesktopEntry( intf_thread_t *p_intf, DBusMessageIter *container )
{
    VLC_UNUSED( p_intf );
    const char* psz_ret = PACKAGE;
    if (!dbus_message_iter_append_basic( container, DBUS_TYPE_STRING, &psz_ret ))
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
