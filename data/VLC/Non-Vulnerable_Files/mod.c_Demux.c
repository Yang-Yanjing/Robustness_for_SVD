 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t     *p_frame;
    const int i_bk = ( p_sys->fmt.audio.i_bitspersample / 8 ) *
                       p_sys->fmt.audio.i_channels;
    p_frame = block_Alloc( p_sys->fmt.audio.i_rate / 10 * i_bk );
    if( !p_frame )
        return -1;
    const int i_read = ModPlug_Read( p_sys->f, p_frame->p_buffer, p_frame->i_buffer );
    if( i_read <= 0 )
    {
        /* EOF */
        block_Release( p_frame );
        return 0;
    }
    p_frame->i_buffer = i_read;
    p_frame->i_dts =
    p_frame->i_pts = VLC_TS_0 + date_Get( &p_sys->pts );
    /* Set PCR */
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_frame->i_pts );
    /* Send data */
    es_out_Send( p_demux->out, p_sys->es, p_frame );
    date_Increment( &p_sys->pts, i_read / i_bk );
    return 1;
}
