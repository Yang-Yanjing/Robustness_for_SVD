static int
MarshalCanSeek( intf_thread_t *p_intf, DBusMessageIter *container )
{
    dbus_bool_t b_can_seek = FALSE;
    input_thread_t *p_input = pl_CurrentInput( p_intf );
    if( p_input )
    {
        b_can_seek = var_GetBool( p_input, "can-seek" );
        vlc_object_release( p_input );
    }
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_BOOLEAN,
                                         &b_can_seek ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
