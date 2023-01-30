}
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t *p_blockv = NULL;
    /* Try grabbing video frame */
    p_blockv = GrabVideo( p_demux );
    if( !p_blockv )
    {
        /* Sleep so we do not consume all the cpu, 10ms seems
         * like a good value (100fps)
         */
        msleep( 10000 );
        return 1;
    }
    if( p_blockv )
    {
        es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_blockv->i_pts );
        es_out_Send( p_demux->out, p_sys->p_es_video, p_blockv );
    }
    return 1;
}
