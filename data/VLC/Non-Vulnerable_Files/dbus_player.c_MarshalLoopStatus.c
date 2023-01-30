static int
MarshalLoopStatus( intf_thread_t *p_intf, DBusMessageIter *container )
{
    const char *psz_loop_status;
    if( var_GetBool( p_intf->p_sys->p_playlist, "repeat" ) )
        psz_loop_status = LOOP_STATUS_TRACK;
    else if( var_GetBool( p_intf->p_sys->p_playlist, "loop" ) )
        psz_loop_status = LOOP_STATUS_PLAYLIST;
    else
        psz_loop_status = LOOP_STATUS_NONE;
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_STRING,
                                         &psz_loop_status ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
