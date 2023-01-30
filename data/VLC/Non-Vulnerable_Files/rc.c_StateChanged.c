}
static void StateChanged( intf_thread_t *p_intf, input_thread_t *p_input )
{
    playlist_t *p_playlist = p_intf->p_sys->p_playlist;
    PL_LOCK;
    const int i_status = playlist_Status( p_playlist );
    PL_UNLOCK;
    /* */
    const char *psz_cmd;
    switch( i_status )
    {
    case PLAYLIST_STOPPED:
        psz_cmd = "stop";
        break;
    case PLAYLIST_RUNNING:
        psz_cmd = "play";
        break;
    case PLAYLIST_PAUSED:
        psz_cmd = "pause";
        break;
    default:
        psz_cmd = "";
        break;
    }
    /* */
    const int i_state = var_GetInteger( p_input, "state" );
    vlc_mutex_lock( &p_intf->p_sys->status_lock );
    msg_rc( STATUS_CHANGE "( %s state: %d ): %s", psz_cmd,
            i_state, ppsz_input_state[i_state] );
    vlc_mutex_unlock( &p_intf->p_sys->status_lock );
}
