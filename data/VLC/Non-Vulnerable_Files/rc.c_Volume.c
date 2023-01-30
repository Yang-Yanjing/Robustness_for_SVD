}
static int Volume( vlc_object_t *p_this, char const *psz_cmd,
                   vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    intf_thread_t *p_intf = (intf_thread_t*)p_this;
    playlist_t *p_playlist = p_intf->p_sys->p_playlist;
    input_thread_t *p_input = playlist_CurrentInput( p_playlist );
    int i_error = VLC_EGENERIC;
    if( !p_input )
        return VLC_ENOOBJ;
    if( p_input )
    {
        int state = var_GetInteger( p_input, "state" );
        vlc_object_release( p_input );
        if( state == PAUSE_S )
        {
            msg_rc( "%s", _("Type 'pause' to continue.") );
            return VLC_EGENERIC;
        }
    }
    if ( *newval.psz_string )
    {
        /* Set. */
        int i_volume = atoi( newval.psz_string );
        if( !playlist_VolumeSet( p_playlist,
                             i_volume / (float)AOUT_VOLUME_DEFAULT ) )
            i_error = VLC_SUCCESS;
        playlist_MuteSet( p_playlist, i_volume == 0 );
        msg_rc( STATUS_CHANGE "( audio volume: %d )", i_volume );
    }
    else
    {
        /* Get. */
        msg_rc( STATUS_CHANGE "( audio volume: %ld )",
               lroundf( playlist_VolumeGet( p_playlist ) * AOUT_VOLUME_DEFAULT ) );
        i_error = VLC_SUCCESS;
    }
    return i_error;
}
