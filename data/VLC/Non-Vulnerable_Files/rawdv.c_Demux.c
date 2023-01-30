 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    block_t     *p_block;
    bool  b_audio = false;
    if( p_sys->b_hurry_up )
    {
         /* 3 frames */
        p_sys->i_pcr = mdate() + (p_sys->i_dsf ? 120000 : 90000);
    }
    /* Call the pace control */
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, VLC_TS_0 + p_sys->i_pcr );
    p_block = stream_Block( p_demux->s, p_sys->frame_size );
    if( p_block == NULL )
    {
        /* EOF */
        return 0;
    }
    if( p_sys->p_es_audio )
    {
        es_out_Control( p_demux->out, ES_OUT_GET_ES_STATE,
                        p_sys->p_es_audio, &b_audio );
    }
    p_block->i_dts =
    p_block->i_pts = VLC_TS_0 + p_sys->i_pcr;
    if( b_audio )
    {
        block_t *p_audio_block = dv_extract_audio( p_block );
        if( p_audio_block )
            es_out_Send( p_demux->out, p_sys->p_es_audio, p_audio_block );
    }
    es_out_Send( p_demux->out, p_sys->p_es_video, p_block );
    if( !p_sys->b_hurry_up )
    {
        p_sys->i_pcr += ( INT64_C(1000000) / p_sys->f_rate );
    }
    return 1;
}
