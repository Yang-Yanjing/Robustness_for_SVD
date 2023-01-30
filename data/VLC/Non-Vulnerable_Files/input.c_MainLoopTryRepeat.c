}
static int MainLoopTryRepeat( input_thread_t *p_input, mtime_t *pi_start_mdate )
{
    int i_repeat = var_GetInteger( p_input, "input-repeat" );
    if( i_repeat == 0 )
        return VLC_EGENERIC;
    vlc_value_t val;
    msg_Dbg( p_input, "repeating the same input (%d)", i_repeat );
    if( i_repeat > 0 )
    {
        i_repeat--;
        var_SetInteger( p_input, "input-repeat", i_repeat );
    }
    /* Seek to start title/seekpoint */
    val.i_int = p_input->p->input.i_title_start -
        p_input->p->input.i_title_offset;
    if( val.i_int < 0 || val.i_int >= p_input->p->input.i_title )
        val.i_int = 0;
    input_ControlPush( p_input,
                       INPUT_CONTROL_SET_TITLE, &val );
    val.i_int = p_input->p->input.i_seekpoint_start -
        p_input->p->input.i_seekpoint_offset;
    if( val.i_int > 0 /* TODO: check upper boundary */ )
        input_ControlPush( p_input,
                           INPUT_CONTROL_SET_SEEKPOINT, &val );
    /* Seek to start position */
    if( p_input->p->i_start > 0 )
    {
        val.i_time = p_input->p->i_start;
        input_ControlPush( p_input, INPUT_CONTROL_SET_TIME, &val );
    }
    else
    {
        val.f_float = 0.0;
        input_ControlPush( p_input, INPUT_CONTROL_SET_POSITION, &val );
    }
    /* */
    *pi_start_mdate = mdate();
    return VLC_SUCCESS;
}
