}
static int VideoSplitterCallback( vlc_object_t *p_this, char const *psz_cmd,
                                  vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    playlist_t *p_playlist = (playlist_t*)p_this;
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval); VLC_UNUSED(p_data); VLC_UNUSED(newval);
    PL_LOCK;
    /* Force the input to restart the video ES to force a vout recreation */
    input_thread_t *p_input = pl_priv( p_playlist )->p_input;
    if( p_input )
    {
        const double f_position = var_GetFloat( p_input, "position" );
        input_Control( p_input, INPUT_RESTART_ES, -VIDEO_ES );
        var_SetFloat( p_input, "position", f_position );
    }
    PL_UNLOCK;
    return VLC_SUCCESS;
}
