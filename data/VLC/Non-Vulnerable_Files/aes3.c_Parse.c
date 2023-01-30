};
static block_t *Parse( decoder_t *p_dec, int *pi_frame_length, int *pi_bits,
                       block_t **pp_block, bool b_packetizer )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_block;
    uint32_t h;
    unsigned int i_size;
    int i_channels;
    int i_bits;
    if( !pp_block || !*pp_block ) return NULL;
    p_block = *pp_block;
    *pp_block = NULL; /* So the packet doesn't get re-sent */
    /* Date management */
    if( p_block->i_pts > VLC_TS_INVALID &&
        p_block->i_pts != date_Get( &p_sys->end_date ) )
    {
        date_Set( &p_sys->end_date, p_block->i_pts );
    }
    if( !date_Get( &p_sys->end_date ) )
    {
        /* We've just started the stream, wait for the first PTS. */
        block_Release( p_block );
        return NULL;
    }
    if( p_block->i_buffer <= AES3_HEADER_LEN )
    {
        msg_Err(p_dec, "frame is too short");
        block_Release( p_block );
        return NULL;
    }
    /*
     * AES3 header :
     * size:            16
     * number channels   2
     * channel_id        8
     * bits per samples  2
     * alignments        4
     */
    h = GetDWBE( p_block->p_buffer );
    i_size = (h >> 16) & 0xffff;
    i_channels = 2 + 2*( (h >> 14) & 0x03 );
    i_bits = 16 + 4*( (h >> 4)&0x03 );
    if( AES3_HEADER_LEN + i_size != p_block->i_buffer || i_bits > 24 )
    {
        msg_Err(p_dec, "frame has invalid header");
        block_Release( p_block );
        return NULL;
    }
    /* Set output properties */
    if( b_packetizer )
    {
        p_dec->fmt_out.audio.i_bitspersample = i_bits;
    }
    else
    {
        p_dec->fmt_out.i_codec = i_bits == 16 ? VLC_CODEC_S16N : VLC_CODEC_S32N;
        p_dec->fmt_out.audio.i_bitspersample = i_bits == 16 ? 16 : 32;
    }
    p_dec->fmt_out.audio.i_channels = i_channels;
    p_dec->fmt_out.audio.i_original_channels = pi_original_channels[i_channels/2-1];
    p_dec->fmt_out.audio.i_physical_channels = pi_original_channels[i_channels/2-1];
    *pi_frame_length = (p_block->i_buffer - AES3_HEADER_LEN) / ( (4+i_bits) * i_channels / 8 );
    *pi_bits = i_bits;
    return p_block;
}
