 *****************************************************************************/
static int StateCallback( vlc_object_t *p_this, char const *psz_cmd,
                          vlc_value_t oldval, vlc_value_t newval,
                          void *p_data )
{
    input_thread_t *p_input = (input_thread_t*)p_this;
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    if( newval.i_int == PLAYING_S || newval.i_int == PAUSE_S )
    {
        input_ControlPush( p_input, INPUT_CONTROL_SET_STATE, &newval );
        return VLC_SUCCESS;
    }
    return VLC_EGENERIC;
}