static int
MarshalCanPause( intf_thread_t *p_intf, DBusMessageIter *container )
{
    dbus_bool_t b_can_pause = FALSE;
    input_thread_t *p_input = pl_CurrentInput( p_intf );
    if( p_input )
    {
        b_can_pause = var_GetBool( p_input, "can-pause" );
        vlc_object_release( p_input );
    }
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_BOOLEAN,
                                         &b_can_pause ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
