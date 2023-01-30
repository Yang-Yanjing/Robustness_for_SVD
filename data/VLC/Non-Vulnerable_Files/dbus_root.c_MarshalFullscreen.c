static int
MarshalFullscreen( intf_thread_t *p_intf, DBusMessageIter *container )
{
    dbus_bool_t b_fullscreen;
    if ( p_intf->p_sys->p_playlist )
        b_fullscreen = var_GetBool( p_intf->p_sys->p_playlist , "fullscreen" );
    else
        b_fullscreen = FALSE;
    if (!dbus_message_iter_append_basic( container,
            DBUS_TYPE_BOOLEAN, &b_fullscreen ))
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
