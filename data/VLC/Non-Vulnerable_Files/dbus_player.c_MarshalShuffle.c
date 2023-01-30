static int
MarshalShuffle( intf_thread_t *p_intf, DBusMessageIter *container )
{
    dbus_bool_t b_shuffle = var_GetBool( p_intf->p_sys->p_playlist, "random" );
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_BOOLEAN,
                                         &b_shuffle ))
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
