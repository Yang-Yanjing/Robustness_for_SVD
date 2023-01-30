static int
MarshalCanPlay( intf_thread_t *p_intf, DBusMessageIter *container )
{
    playlist_t *p_playlist = p_intf->p_sys->p_playlist;
    PL_LOCK;
    dbus_bool_t b_can_play = playlist_CurrentSize( p_playlist ) > 0;
    PL_UNLOCK;
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_BOOLEAN,
                                         &b_can_play ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
