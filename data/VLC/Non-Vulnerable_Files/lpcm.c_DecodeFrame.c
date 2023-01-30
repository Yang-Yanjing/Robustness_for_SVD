 *****************************************************************************/
static block_t *DecodeFrame( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_block;
    unsigned int  i_rate = 0, i_original_channels = 0, i_channels = 0, i_bits = 0;
    int           i_frame_length;
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
    if( p_block->i_buffer <= p_sys->i_header_size )
    {
        msg_Err(p_dec, "frame is too short");
        block_Release( p_block );
        return NULL;
    }
    int i_ret;
    unsigned i_channels_padding = 0;
    unsigned i_padding = 0;
    aob_group_t p_aob_group[2];
    switch( p_sys->i_type )
    {
    case LPCM_VOB:
        i_ret = VobHeader( &i_rate, &i_channels, &i_original_channels, &i_bits,
                           p_block->p_buffer );
        break;
    case LPCM_AOB:
        i_ret = AobHeader( &i_rate, &i_channels, &i_original_channels, &i_bits, &i_padding,
                           p_aob_group,
                           p_block->p_buffer );
        break;
    case LPCM_BD:
        i_ret = BdHeader( p_sys, &i_rate, &i_channels, &i_channels_padding, &i_original_channels, &i_bits,
                          p_block->p_buffer );
        break;
    case LPCM_WIDI:
        i_ret = WidiHeader( &i_rate, &i_channels, &i_original_channels, &i_bits,
                            p_block->p_buffer );
        break;
    default:
        abort();
    }
    if( i_ret || p_block->i_buffer <= p_sys->i_header_size + i_padding )
    {
        msg_Warn( p_dec, "no frame sync or too small frame" );
        block_Release( p_block );
        return NULL;
    }
    /* Set output properties */
    if( p_dec->fmt_out.audio.i_rate != i_rate )
    {
        date_Init( &p_sys->end_date, i_rate, 1 );
        date_Set( &p_sys->end_date, p_block->i_pts );
    }
    p_dec->fmt_out.audio.i_rate = i_rate;
    p_dec->fmt_out.audio.i_channels = i_channels;
    p_dec->fmt_out.audio.i_original_channels = i_original_channels;
    p_dec->fmt_out.audio.i_physical_channels = i_original_channels;
    i_frame_length = (p_block->i_buffer - p_sys->i_header_size - i_padding) /
                     (i_channels + i_channels_padding) * 8 / i_bits;
    if( p_sys->b_packetizer )
    {
        p_block->i_pts = p_block->i_dts = date_Get( &p_sys->end_date );
        p_block->i_length =
            date_Increment( &p_sys->end_date, i_frame_length ) -
            p_block->i_pts;
        /* Just pass on the incoming frame */
        return p_block;
    }
    else
    {
        /* */
        if( i_bits == 16 )
        {
            p_dec->fmt_out.audio.i_format =
            p_dec->fmt_out.i_codec = VLC_CODEC_S16N;
            p_dec->fmt_out.audio.i_bitspersample = 16;
        }
        else
        {
            p_dec->fmt_out.audio.i_format =
            p_dec->fmt_out.i_codec = VLC_CODEC_S32N;
            p_dec->fmt_out.audio.i_bitspersample = 32;
        }
        /* */
        block_t *p_aout_buffer;
        p_aout_buffer = decoder_NewAudioBuffer( p_dec, i_frame_length );
        if( !p_aout_buffer )
            return NULL;
        p_aout_buffer->i_pts = date_Get( &p_sys->end_date );
        p_aout_buffer->i_length =
            date_Increment( &p_sys->end_date, i_frame_length )
            - p_aout_buffer->i_pts;
        p_block->p_buffer += p_sys->i_header_size + i_padding;
        p_block->i_buffer -= p_sys->i_header_size + i_padding;
        switch( p_sys->i_type )
        {
        case LPCM_WIDI:
        case LPCM_VOB:
            VobExtract( p_aout_buffer, p_block, i_bits );
            break;
        case LPCM_AOB:
            AobExtract( p_aout_buffer, p_block, i_bits, p_aob_group );
            break;
        default:
            assert(0);
        case LPCM_BD:
            BdExtract( p_aout_buffer, p_block, i_frame_length, i_channels, i_channels_padding, i_bits );
            break;
        }
        if( p_sys->i_chans_to_reorder )
        {
            aout_ChannelReorder( p_aout_buffer->p_buffer, p_aout_buffer->i_buffer,
                                 p_sys->i_chans_to_reorder, p_sys->pi_chan_table,
                                 p_dec->fmt_out.i_codec );
        }
        block_Release( p_block );
        return p_aout_buffer;
    }
}
