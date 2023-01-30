static int
MarshalCanSetFullscreen( intf_thread_t *p_intf, DBusMessageIter *container )
{
    input_thread_t *p_input = NULL;
    dbus_bool_t     b_ret   = FALSE;
    if (p_intf->p_sys->p_input)
    {
        p_input = (input_thread_t*) vlc_object_hold( p_intf->p_sys->p_input );
        vout_thread_t* p_vout = input_GetVout( p_input );
        vlc_object_release( p_input );
        if ( p_vout )
        {
            b_ret = TRUE;
            vlc_object_release( p_vout );
        }
    }
    if (!dbus_message_iter_append_basic( container, DBUS_TYPE_BOOLEAN, &b_ret ))
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
