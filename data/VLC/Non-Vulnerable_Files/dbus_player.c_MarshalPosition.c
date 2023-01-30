static int
MarshalPosition( intf_thread_t *p_intf, DBusMessageIter *container )
{
    /* returns position in microseconds */
    dbus_int64_t i_pos;
    input_thread_t *p_input = pl_CurrentInput( p_intf );
    if( !p_input )
        i_pos = 0;
    else
    {
        i_pos = var_GetTime( p_input, "time" );
        vlc_object_release( p_input );
    }
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_INT64, &i_pos ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
