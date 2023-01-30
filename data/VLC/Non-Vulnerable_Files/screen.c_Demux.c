 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t *p_block;
    if( !p_sys->i_next_date ) p_sys->i_next_date = mdate();
    /* Frame skipping if necessary */
    while( mdate() >= p_sys->i_next_date + p_sys->i_incr )
        p_sys->i_next_date += p_sys->i_incr;
    mwait( p_sys->i_next_date );
    p_block = screen_Capture( p_demux );
    if( !p_block )
    {
        p_sys->i_next_date += p_sys->i_incr;
        return 1;
    }
    p_block->i_dts = p_block->i_pts = p_sys->i_next_date;
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_block->i_pts );
    es_out_Send( p_demux->out, p_sys->es, p_block );
    p_sys->i_next_date += p_sys->i_incr;
    return 1;
}
