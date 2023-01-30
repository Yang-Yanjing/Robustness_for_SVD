static int
MarshalMaximumRate( intf_thread_t *p_intf, DBusMessageIter *container )
{
    VLC_UNUSED( p_intf );
    double d_max_rate = (double) INPUT_RATE_MAX / INPUT_RATE_DEFAULT;
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_DOUBLE, &d_max_rate ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
