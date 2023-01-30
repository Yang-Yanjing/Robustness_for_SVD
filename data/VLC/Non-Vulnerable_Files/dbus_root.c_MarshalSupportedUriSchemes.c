static int
MarshalSupportedUriSchemes( intf_thread_t *p_intf, DBusMessageIter *container )
{
    VLC_UNUSED( p_intf );
    DBusMessageIter ret;
    size_t i_len = sizeof( ppsz_supported_uri_schemes ) /
        sizeof( *ppsz_supported_uri_schemes );
    if( !dbus_message_iter_open_container( container,
                                           DBUS_TYPE_ARRAY, "s",
                                           &ret ) )
        return VLC_ENOMEM;
    for( size_t i = 0; i < i_len; ++i )
    {
        const char* const psz_scheme = ppsz_supported_uri_schemes[i];
        if( !dbus_message_iter_append_basic( &ret, DBUS_TYPE_STRING,
                                             &psz_scheme ) )
            return VLC_ENOMEM;
    }
    if( !dbus_message_iter_close_container( container, &ret ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
