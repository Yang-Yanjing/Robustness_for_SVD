}
static void SlaveDemux( input_thread_t *p_input, bool *pb_demux_polled )
{
    int64_t i_time;
    int i;
    *pb_demux_polled = false;
    if( demux_Control( p_input->p->input.p_demux, DEMUX_GET_TIME, &i_time ) )
    {
        msg_Err( p_input, "demux doesn't like DEMUX_GET_TIME" );
        return;
    }
    for( i = 0; i < p_input->p->i_slave; i++ )
    {
        input_source_t *in = p_input->p->slave[i];
        int i_ret;
        if( in->b_eof )
            continue;
        const bool b_demux_polled = in->p_demux->pf_demux != NULL;
        if( !b_demux_polled )
            continue;
        *pb_demux_polled = true;
        /* Call demux_Demux until we have read enough data */
        if( demux_Control( in->p_demux, DEMUX_SET_NEXT_DEMUX_TIME, i_time ) )
        {
            for( ;; )
            {
                int64_t i_stime;
                if( demux_Control( in->p_demux, DEMUX_GET_TIME, &i_stime ) )
                {
                    msg_Err( p_input, "slave[%d] doesn't like "
                             "DEMUX_GET_TIME -> EOF", i );
                    i_ret = 0;
                    break;
                }
                if( i_stime >= i_time )
                {
                    i_ret = 1;
                    break;
                }
                if( ( i_ret = demux_Demux( in->p_demux ) ) <= 0 )
                    break;
            }
        }
        else
        {
            i_ret = demux_Demux( in->p_demux );
        }
        if( i_ret <= 0 )
        {
            msg_Dbg( p_input, "slave %d EOF", i );
            in->b_eof = true;
        }
    }
}
