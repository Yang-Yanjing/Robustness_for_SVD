}
static void ControlUnpause( input_thread_t *p_input, mtime_t i_control_date )
{
    int i_ret = VLC_SUCCESS;
    if( p_input->p->b_can_pause )
    {
        demux_t *p_demux = p_input->p->input.p_demux;
        if( p_demux->s != NULL )
            i_ret = stream_Control( p_demux->s, STREAM_SET_PAUSE_STATE, false );
        else
            i_ret = demux_Control( p_demux, DEMUX_SET_PAUSE_STATE, false );
        if( i_ret )
        {
            /* FIXME What to do ? */
            msg_Warn( p_input, "cannot unset pause -> EOF" );
            input_ControlPush( p_input, INPUT_CONTROL_SET_DIE, NULL );
        }
    }
    /* Switch to play */
    input_ChangeState( p_input, PLAYING_S );
    /* */
    if( !i_ret )
        es_out_SetPauseState( p_input->p->p_es_out, false, false, i_control_date );
}
