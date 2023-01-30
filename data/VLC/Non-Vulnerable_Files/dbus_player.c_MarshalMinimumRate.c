static int
MarshalMinimumRate( intf_thread_t *p_intf, DBusMessageIter *container )
{
    VLC_UNUSED( p_intf );
    double d_min_rate = (double) INPUT_RATE_MIN / INPUT_RATE_DEFAULT;
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_DOUBLE, &d_min_rate ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
