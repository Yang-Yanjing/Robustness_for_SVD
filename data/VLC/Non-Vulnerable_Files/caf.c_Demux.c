 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t     *p_block;
    if( p_sys->i_data_size != kCHUNK_SIZE_EOF && p_sys->position.i_bytes >= p_sys->i_data_size )
    {
        /* EOF */
        return 0;
    }
    frame_span_t advance = (frame_span_t){0};
    /* we will read 50ms at once */
    uint64_t i_req_samples = __MAX( p_sys->fmt.audio.i_rate / 20, 1 );
    if( !NeedsPacketTable( p_sys )) /* PCM/IMA4 */
    {
        int64_t i_req_frames = ( i_req_samples + ( p_sys->fmt.audio.i_frame_length - 1 )) / p_sys->fmt.audio.i_frame_length;
        if( p_sys->i_data_size != kCHUNK_SIZE_EOF && ( p_sys->position.i_bytes + i_req_frames * p_sys->fmt.audio.i_bytes_per_frame ) > p_sys->i_data_size )
        {
            i_req_frames = ( p_sys->i_data_size - p_sys->position.i_frames * p_sys->fmt.audio.i_bytes_per_frame ) / p_sys->fmt.audio.i_bytes_per_frame;
        }
        advance.i_frames = i_req_frames;
        advance.i_samples = i_req_frames * p_sys->fmt.audio.i_frame_length;
        advance.i_bytes = p_sys->fmt.audio.i_bytes_per_frame * advance.i_frames;
    }
    else /* use packet table */
    {
        do
        {
            if( FrameSpanAddDescription( p_demux, p_sys->position.i_desc_bytes + advance.i_desc_bytes, &advance ))
                break;
        }
        while (( i_req_samples > advance.i_samples ) && ( p_sys->position.i_frames + advance.i_frames ) < p_sys->packet_table.i_num_packets );
    }
    if( !advance.i_frames )
    {
        msg_Err( p_demux, "Unexpected end of file" );
        return -1;
    }
    if( stream_Seek( p_demux->s, p_sys->i_data_offset + p_sys->position.i_bytes ))
    {
        if( p_sys->i_data_size == kCHUNK_SIZE_EOF)
            return 0;
        msg_Err( p_demux, "cannot seek data" );
        return -1;
    }
    if(( p_block = stream_Block( p_demux->s, (int)advance.i_bytes )) == NULL )
    {
        msg_Err( p_demux, "cannot read data" );
        return -1;
    }
    p_block->i_dts =
    p_block->i_pts = VLC_TS_0 + FrameSpanGetTime( &p_sys->position, p_sys->fmt.audio.i_rate );
    FrameSpanAddSpan( &p_sys->position, &advance );
    /* set PCR */
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_block->i_pts );
    es_out_Send( p_demux->out, p_sys->es, p_block );
    return 1;
}
