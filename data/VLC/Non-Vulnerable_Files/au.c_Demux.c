 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t     *p_block;
    /* set PCR */
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, VLC_TS_0 + p_sys->i_time );
    if( ( p_block = stream_Block( p_demux->s, p_sys->i_frame_size ) ) == NULL )
    {
        msg_Warn( p_demux, "cannot read data" );
        return 0;
    }
    p_block->i_dts =
    p_block->i_pts = VLC_TS_0 + p_sys->i_time;
    es_out_Send( p_demux->out, p_sys->es, p_block );
    p_sys->i_time += p_sys->i_frame_length;
    return 1;
}
