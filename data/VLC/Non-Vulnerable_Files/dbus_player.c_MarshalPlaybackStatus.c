static int
MarshalPlaybackStatus( intf_thread_t *p_intf, DBusMessageIter *container )
{
    input_thread_t *p_input = pl_CurrentInput( p_intf );
    const char *psz_playback_status;
    if( p_input != NULL )
    {
        switch( var_GetInteger( p_input, "state" ) )
        {
            case OPENING_S:
            case PLAYING_S:
                psz_playback_status = PLAYBACK_STATUS_PLAYING;
                break;
            case PAUSE_S:
                psz_playback_status = PLAYBACK_STATUS_PAUSED;
                break;
            default:
                psz_playback_status = PLAYBACK_STATUS_STOPPED;
        }
        vlc_object_release( (vlc_object_t*) p_input );
    }
    else
        psz_playback_status = PLAYBACK_STATUS_STOPPED;
    if( !dbus_message_iter_append_basic( container, DBUS_TYPE_STRING,
                                         &psz_playback_status ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
