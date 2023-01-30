static int
MarshalVolume( intf_thread_t *p_intf, DBusMessageIter *container )
{
    float f_vol = playlist_VolumeGet( p_intf->p_sys->p_playlist );
    if( f_vol < 0.f )
        f_vol = 1.f; /* ? */
    double d_vol = f_vol;
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_DOUBLE, &d_vol ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
