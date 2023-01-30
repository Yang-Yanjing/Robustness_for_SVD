}
static void StartTitle( input_thread_t * p_input )
{
    vlc_value_t val;
    /* Start title/chapter */
    val.i_int = p_input->p->input.i_title_start -
                p_input->p->input.i_title_offset;
    if( val.i_int > 0 && val.i_int < p_input->p->input.i_title )
        input_ControlPush( p_input, INPUT_CONTROL_SET_TITLE, &val );
    val.i_int = p_input->p->input.i_seekpoint_start -
                p_input->p->input.i_seekpoint_offset;
    if( val.i_int > 0 /* TODO: check upper boundary */ )
        input_ControlPush( p_input, INPUT_CONTROL_SET_SEEKPOINT, &val );
    /* Start/stop/run time */
    p_input->p->i_start = (int64_t)(1000000.0
                                     * var_GetFloat( p_input, "start-time" ));
    p_input->p->i_stop  = (int64_t)(1000000.0
                                     * var_GetFloat( p_input, "stop-time" ));
    p_input->p->i_run   = (int64_t)(1000000.0
                                     * var_GetFloat( p_input, "run-time" ));
    if( p_input->p->i_run < 0 )
    {
        msg_Warn( p_input, "invalid run-time ignored" );
        p_input->p->i_run = 0;
    }
    if( p_input->p->i_start > 0 )
    {
        vlc_value_t s;
        msg_Dbg( p_input, "starting at time: %ds",
                 (int)( p_input->p->i_start / INT64_C(1000000) ) );
        s.i_time = p_input->p->i_start;
        input_ControlPush( p_input, INPUT_CONTROL_SET_TIME, &s );
    }
    if( p_input->p->i_stop > 0 && p_input->p->i_stop <= p_input->p->i_start )
    {
        msg_Warn( p_input, "invalid stop-time ignored" );
        p_input->p->i_stop = 0;
    }
    p_input->p->b_fast_seek = var_GetBool( p_input, "input-fast-seek" );
}
