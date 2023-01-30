 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    int64_t     i_tell = stream_Tell( p_demux->s );
    block_t     *p_block;
    int         i_read;
    if( p_sys->i_ssnd_end > 0 && i_tell >= p_sys->i_ssnd_end )
    {
        /* EOF */
        return 0;
    }
    /* Set PCR */
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, VLC_TS_0 + p_sys->i_time);
    /* we will read 100ms at once */
    i_read = p_sys->i_ssnd_fsize * ( p_sys->fmt.audio.i_rate / 10 );
    if( p_sys->i_ssnd_end > 0 && p_sys->i_ssnd_end - i_tell < i_read )
    {
        i_read = p_sys->i_ssnd_end - i_tell;
    }
    if( ( p_block = stream_Block( p_demux->s, i_read ) ) == NULL )
    {
        return 0;
    }
    p_block->i_dts =
    p_block->i_pts = VLC_TS_0 + p_sys->i_time;
    p_sys->i_time += (int64_t)1000000 *
                     p_block->i_buffer /
                     p_sys->i_ssnd_fsize /
                     p_sys->fmt.audio.i_rate;
    /* */
    es_out_Send( p_demux->out, p_sys->es, p_block );
    return 1;
}
