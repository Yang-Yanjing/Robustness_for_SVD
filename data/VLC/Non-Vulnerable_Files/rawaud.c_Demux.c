 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    block_t     *p_block;
    if( ( p_block = stream_Block( p_demux->s, p_sys->i_frame_size ) ) == NULL )
    {
        /* EOF */
        return 0;
    }
    p_block->i_dts =
    p_block->i_pts = VLC_TS_0 + date_Get( &p_sys->pts );
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_block->i_pts );
    es_out_Send( p_demux->out, p_sys->p_es, p_block );
    date_Increment( &p_sys->pts, p_sys->i_frame_samples );
    return 1;
}
