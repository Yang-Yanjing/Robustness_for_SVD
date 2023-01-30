}
static int VolumeMove( vlc_object_t *p_this, char const *psz_cmd,
                       vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    intf_thread_t *p_intf = (intf_thread_t*)p_this;
    float volume;
    input_thread_t *p_input =
        playlist_CurrentInput( p_intf->p_sys->p_playlist );
    int i_nb_steps = atoi(newval.psz_string);
    int i_error = VLC_SUCCESS;
    if( !p_input )
        return VLC_ENOOBJ;
    int state = var_GetInteger( p_input, "state" );
    vlc_object_release( p_input );
    if( state == PAUSE_S )
    {
        msg_rc( "%s", _("Type 'pause' to continue.") );
        return VLC_EGENERIC;
    }
    if( !strcmp(psz_cmd, "voldown") )
        i_nb_steps *= -1;
    if( playlist_VolumeUp( p_intf->p_sys->p_playlist, i_nb_steps, &volume ) < 0 )
        i_error = VLC_EGENERIC;
    if ( !i_error )
        msg_rc( STATUS_CHANGE "( audio volume: %ld )",
                lroundf( volume * AOUT_VOLUME_DEFAULT ) );
    return i_error;
}
