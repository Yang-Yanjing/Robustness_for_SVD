}
static void SlaveSeek( input_thread_t *p_input )
{
    int64_t i_time;
    int i;
    if( demux_Control( p_input->p->input.p_demux, DEMUX_GET_TIME, &i_time ) )
    {
        msg_Err( p_input, "demux doesn't like DEMUX_GET_TIME" );
        return;
    }
    for( i = 0; i < p_input->p->i_slave; i++ )
    {
        input_source_t *in = p_input->p->slave[i];
        if( demux_Control( in->p_demux, DEMUX_SET_TIME, i_time, true ) )
        {
            if( !in->b_eof )
                msg_Err( p_input, "seek failed for slave %d -> EOF", i );
            in->b_eof = true;
        }
        else
        {
            in->b_eof = false;
        }
    }
}
