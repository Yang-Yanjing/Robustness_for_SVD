static int
MarshalHasTrackList( intf_thread_t *p_intf, DBusMessageIter *container )
{
    VLC_UNUSED( p_intf );
    const dbus_bool_t b_ret = FALSE;
    if (!dbus_message_iter_append_basic( container, DBUS_TYPE_BOOLEAN, &b_ret ))
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
