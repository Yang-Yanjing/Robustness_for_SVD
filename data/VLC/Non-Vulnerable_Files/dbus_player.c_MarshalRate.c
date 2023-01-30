static int
MarshalRate( intf_thread_t *p_intf, DBusMessageIter *container )
{
    double d_rate;
    input_thread_t *p_input = pl_CurrentInput( p_intf );
    if( p_input != NULL )
    {
        d_rate = var_GetFloat( p_input, "rate" );
        vlc_object_release( (vlc_object_t*) p_input );
    }
    else
        d_rate = 1.0;
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_DOUBLE,
                                         &d_rate ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
