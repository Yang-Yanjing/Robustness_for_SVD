/* Adds the frame that is described at i_desc_offset to span */
static int FrameSpanAddDescription( demux_t *p_demux, uint64_t i_desc_offset, frame_span_t *span )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    /* Avoid seeking + peeking for simple case (PCM) */
    if( p_sys->fmt.audio.i_bytes_per_frame && p_sys->fmt.audio.i_frame_length )
    {
        span->i_bytes += p_sys->fmt.audio.i_bytes_per_frame;
        span->i_samples += p_sys->fmt.audio.i_frame_length;
        span->i_frames++;
        return VLC_SUCCESS;
    }
    uint32_t i_desc_size = 0;
    if( stream_Seek( p_demux->s, p_sys->packet_table.i_descriptions_start + i_desc_offset ))
    {
        msg_Err( p_demux, "Couldn't seek packet description." );
        return VLC_EGENERIC;
    }
    const uint8_t *p_peek;
    int i_peek_len = stream_Peek( p_demux->s, &p_peek, 2 * 10 );
    /* Peeking the maximum number of bytes that two 64 bit numbers could use
     * (( 64 + 6 ) / 7 = 10). */
    if( i_peek_len < 0 )
        i_peek_len = 0;
    if( p_sys->fmt.audio.i_bytes_per_frame )
    {
        span->i_bytes += p_sys->fmt.audio.i_bytes_per_frame;
    }
    else
    {
        uint64_t i_size;
        uint32_t i_this_int;
        if( ParseVarLenInteger( p_peek, i_peek_len, &i_size, &i_this_int ))
        {
            return VLC_EGENERIC;
        }
        i_desc_size += i_this_int;
        span->i_bytes += i_size;
    }
    if( p_sys->fmt.audio.i_frame_length )
    {
        span->i_samples += p_sys->fmt.audio.i_frame_length;
    }
    else
    {
        if( i_desc_size >= (unsigned)i_peek_len )
        {
            return VLC_EGENERIC;
        }
        uint64_t i_num_samples;
        uint32_t i_this_int;
        if( ParseVarLenInteger( p_peek + i_desc_size, i_peek_len - i_desc_size, &i_num_samples, &i_this_int ))
        {
            return VLC_EGENERIC;
        }
        i_desc_size += i_this_int;
        span->i_samples += i_num_samples;
    }
    span->i_desc_bytes += i_desc_size;
    span->i_frames++;
    return VLC_SUCCESS;
}
