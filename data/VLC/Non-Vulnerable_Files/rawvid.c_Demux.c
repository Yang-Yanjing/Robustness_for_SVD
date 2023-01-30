 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    block_t     *p_block;
    mtime_t i_pcr = date_Get( &p_sys->pcr );
    /* Call the pace control */
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, VLC_TS_0 + i_pcr );
    if( p_sys->b_y4m )
    {
        /* Skip the frame header */
        /* Skip "FRAME" */
        if( stream_Read( p_demux->s, NULL, 5 ) < 5 )
            return 0;
        /* Find \n */
        for( ;; )
        {
            uint8_t b;
            if( stream_Read( p_demux->s, &b, 1 ) < 1 )
                return 0;
            if( b == 0x0a )
                break;
        }
    }
    if( ( p_block = stream_Block( p_demux->s, p_sys->frame_size ) ) == NULL )
    {
        /* EOF */
        return 0;
    }
    p_block->i_dts = p_block->i_pts = VLC_TS_0 + i_pcr;
    es_out_Send( p_demux->out, p_sys->p_es_video, p_block );
    date_Increment( &p_sys->pcr, 1 );
    return 1;
}
