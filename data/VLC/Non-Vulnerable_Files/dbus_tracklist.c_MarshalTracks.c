static int
MarshalTracks( intf_thread_t *p_intf, DBusMessageIter *container )
{
    DBusMessageIter tracks;
    char         *psz_track_id = NULL;
    playlist_t   *p_playlist   = p_intf->p_sys->p_playlist;
    input_item_t *p_input      = NULL;
    dbus_message_iter_open_container( container, DBUS_TYPE_ARRAY, "o",
                                      &tracks );
    PL_LOCK;
    for( int i = 0; i < playlist_CurrentSize( p_playlist ); i++ )
    {
        p_input = p_playlist->current.p_elems[i]->p_input;
        if( ( -1 == asprintf( &psz_track_id,
                              MPRIS_TRACKID_FORMAT,
                              p_input->i_id ) ) ||
            !dbus_message_iter_append_basic( &tracks,
                                             DBUS_TYPE_OBJECT_PATH,
                                             &psz_track_id ) )
        {
            PL_UNLOCK;
            dbus_message_iter_abandon_container( container, &tracks );
            return VLC_ENOMEM;
        }
        free( psz_track_id );
    }
    PL_UNLOCK;
    if( !dbus_message_iter_close_container( container, &tracks ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
