/* Pause input */
static void ControlPause( input_thread_t *p_input, mtime_t i_control_date )
{
    int i_ret = VLC_SUCCESS;
    int i_state = PAUSE_S;
    if( p_input->p->b_can_pause )
    {
        demux_t *p_demux = p_input->p->input.p_demux;
        if( p_demux->s != NULL )
            i_ret = stream_Control( p_demux->s, STREAM_SET_PAUSE_STATE, true );
        else
            i_ret = demux_Control( p_demux, DEMUX_SET_PAUSE_STATE, true );
        if( i_ret )
        {
            msg_Warn( p_input, "cannot set pause state" );
            return;
        }
    }
    /* */
    i_ret = es_out_SetPauseState( p_input->p->p_es_out,
                                  p_input->p->b_can_pause, true,
                                  i_control_date );
    if( i_ret )
    {
        msg_Warn( p_input, "cannot set pause state at es_out level" );
        return;
    }
    /* Switch to new state */
    input_ChangeState( p_input, i_state );
}
