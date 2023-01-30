static int
MarshalCanGoPrevious( intf_thread_t *p_intf, DBusMessageIter *container )
{
    VLC_UNUSED( p_intf );
    dbus_bool_t b_can_go_previous = TRUE;
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_BOOLEAN,
                                         &b_can_go_previous ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
