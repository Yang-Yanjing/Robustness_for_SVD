static int
MarshalCanControl( intf_thread_t *p_intf, DBusMessageIter *container )
{
    VLC_UNUSED( p_intf );
    dbus_bool_t b_can_control = TRUE;
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_BOOLEAN,
                                         &b_can_control ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
