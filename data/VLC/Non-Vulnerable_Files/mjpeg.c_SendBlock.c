}
static int SendBlock( demux_t *p_demux, int i )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t     *p_block;
    if( ( p_block = stream_Block( p_demux->s, i ) ) == NULL )
    {
        msg_Warn( p_demux, "cannot read data" );
        return 0;
    }
    if( p_sys->i_frame_length )
    {
        p_block->i_pts = p_sys->i_time;
        p_sys->i_time += p_sys->i_frame_length;
    }
    else
    {
        p_block->i_pts = mdate();
    }
    p_block->i_dts = p_block->i_pts;
    /* set PCR */
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_block->i_pts );
    es_out_Send( p_demux->out, p_sys->p_es, p_block );
    if( p_sys->b_still )
        p_sys->i_still_end = mdate() + p_sys->i_frame_length;
    return 1;
}
