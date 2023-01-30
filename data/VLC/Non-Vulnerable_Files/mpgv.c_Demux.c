 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t  *p_sys = p_demux->p_sys;
    block_t *p_block_in, *p_block_out;
    if( ( p_block_in = stream_Block( p_demux->s, MPGV_PACKET_SIZE ) ) == NULL )
    {
        return 0;
    }
    if( p_sys->b_start )
    {
        p_block_in->i_pts =
        p_block_in->i_dts = VLC_TS_0;
    }
    else
    {
        p_block_in->i_pts =
        p_block_in->i_dts = VLC_TS_INVALID;
    }
    while( (p_block_out = p_sys->p_packetizer->pf_packetize( p_sys->p_packetizer, &p_block_in )) )
    {
        p_sys->b_start = false;
        while( p_block_out )
        {
            block_t *p_next = p_block_out->p_next;
            es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_block_out->i_dts );
            p_block_out->p_next = NULL;
            es_out_Send( p_demux->out, p_sys->p_es, p_block_out );
            p_block_out = p_next;
        }
    }
    return 1;
}
