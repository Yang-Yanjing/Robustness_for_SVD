 ****************************************************************************/
static block_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t p_header[MAD_BUFFER_GUARD];
    uint32_t i_header;
    uint8_t *p_buf;
    block_t *p_out_buffer;
    block_t *p_block = pp_block ? *pp_block : NULL;
    if (p_block) {
        if( p_block->i_flags&(BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
        {
            if( p_block->i_flags&BLOCK_FLAG_CORRUPTED )
            {
                p_sys->i_state = STATE_NOSYNC;
                block_BytestreamEmpty( &p_sys->bytestream );
            }
            date_Set( &p_sys->end_date, 0 );
            block_Release( p_block );
            p_sys->b_discontinuity = true;
            return NULL;
        }
        if( !date_Get( &p_sys->end_date ) && p_block->i_pts <= VLC_TS_INVALID )
        {
            /* We've just started the stream, wait for the first PTS. */
            msg_Dbg( p_dec, "waiting for PTS" );
            block_Release( p_block );
            return NULL;
        }
        block_BytestreamPush( &p_sys->bytestream, p_block );
    } else
        p_sys->i_state = STATE_SEND_DATA; /* return all the data we have left */
    while( 1 )
    {
        switch( p_sys->i_state )
        {
        case STATE_NOSYNC:
            while( block_PeekBytes( &p_sys->bytestream, p_header, 2 )
                   == VLC_SUCCESS )
            {
                /* Look for sync word - should be 0xffe */
                if( p_header[0] == 0xff && (p_header[1] & 0xe0) == 0xe0 )
                {
                    p_sys->i_state = STATE_SYNC;
                    break;
                }
                block_SkipByte( &p_sys->bytestream );
            }
            if( p_sys->i_state != STATE_SYNC )
            {
                block_BytestreamFlush( &p_sys->bytestream );
                /* Need more data */
                return NULL;
            }
        case STATE_SYNC:
            /* New frame, set the Presentation Time Stamp */
            p_sys->i_pts = p_sys->bytestream.p_block->i_pts;
            if( p_sys->i_pts > VLC_TS_INVALID &&
                p_sys->i_pts > date_Get( &p_sys->end_date ) )
            {
                date_Set( &p_sys->end_date, p_sys->i_pts );
            }
            p_sys->i_state = STATE_HEADER;
        case STATE_HEADER:
            /* Get MPGA frame header (MPGA_HEADER_SIZE bytes) */
            if( block_PeekBytes( &p_sys->bytestream, p_header,
                                 MPGA_HEADER_SIZE ) != VLC_SUCCESS )
            {
                /* Need more data */
                return NULL;
            }
            /* Build frame header */
            i_header = GetDWBE(p_header);
            /* Check if frame is valid and get frame info */
            p_sys->i_frame_size = SyncInfo( i_header,
                                            &p_sys->i_channels,
                                            &p_sys->i_channels_conf,
                                            &p_sys->i_rate,
                                            &p_sys->i_bit_rate,
                                            &p_sys->i_frame_length,
                                            &p_sys->i_max_frame_size,
                                            &p_sys->i_layer );
            p_dec->fmt_in.i_profile = p_sys->i_layer;
            if( p_sys->i_frame_size == -1 )
            {
                msg_Dbg( p_dec, "emulated startcode" );
                block_SkipByte( &p_sys->bytestream );
                p_sys->i_state = STATE_NOSYNC;
                p_sys->b_discontinuity = true;
                break;
            }
            if( p_sys->i_bit_rate == 0 )
            {
                /* Free bitrate, but 99% emulated startcode :( */
                if( p_dec->p_sys->i_free_frame_size == MPGA_HEADER_SIZE )
                {
                    msg_Dbg( p_dec, "free bitrate mode");
                }
                /* The -1 below is to account for the frame padding */
                p_sys->i_frame_size = p_sys->i_free_frame_size - 1;
            }
            p_sys->i_state = STATE_NEXT_SYNC;
        case STATE_NEXT_SYNC:
            /* TODO: If p_block == NULL, flush the buffer without checking the
             * next sync word */
            /* Check if next expected frame contains the sync word */
            if( block_PeekOffsetBytes( &p_sys->bytestream,
                                       p_sys->i_frame_size, p_header,
                                       MAD_BUFFER_GUARD ) != VLC_SUCCESS )
            {
                /* Need more data */
                return NULL;
            }
            if( p_header[0] == 0xff && (p_header[1] & 0xe0) == 0xe0 )
            {
                /* Startcode is fine, let's try the header as an extra check */
                int i_next_frame_size;
                unsigned int i_next_channels, i_next_channels_conf;
                unsigned int i_next_rate, i_next_bit_rate;
                unsigned int i_next_frame_length, i_next_max_frame_size;
                unsigned int i_next_layer;
                /* Build frame header */
                i_header = GetDWBE(p_header);
                i_next_frame_size = SyncInfo( i_header,
                                              &i_next_channels,
                                              &i_next_channels_conf,
                                              &i_next_rate,
                                              &i_next_bit_rate,
                                              &i_next_frame_length,
                                              &i_next_max_frame_size,
                                              &i_next_layer );
                /* Free bitrate only */
                if( p_sys->i_bit_rate == 0 && i_next_frame_size == -1 )
                {
                    if( (unsigned int)p_sys->i_frame_size >
                        p_sys->i_max_frame_size )
                    {
                        msg_Dbg( p_dec, "frame too big %d > %d "
                                 "(emulated startcode ?)", p_sys->i_frame_size,
                                 p_sys->i_max_frame_size );
                        block_SkipByte( &p_sys->bytestream );
                        p_sys->i_state = STATE_NOSYNC;
                        p_sys->i_free_frame_size = MPGA_HEADER_SIZE;
                        break;
                    }
                    p_sys->i_frame_size++;
                    break;
                }
                if( i_next_frame_size == -1 )
                {
                    msg_Dbg( p_dec, "emulated startcode on next frame" );
                    block_SkipByte( &p_sys->bytestream );
                    p_sys->i_state = STATE_NOSYNC;
                    p_sys->b_discontinuity = true;
                    break;
                }
                /* Check info is in sync with previous one */
                if( i_next_channels_conf != p_sys->i_channels_conf ||
                    i_next_rate != p_sys->i_rate ||
                    i_next_layer != p_sys->i_layer ||
                    i_next_frame_length != p_sys->i_frame_length )
                {
                    /* Free bitrate only */
                    if( p_sys->i_bit_rate == 0 )
                    {
                        p_sys->i_frame_size++;
                        break;
                    }
                    msg_Dbg( p_dec, "parameters changed unexpectedly "
                             "(emulated startcode ?)" );
                    block_SkipByte( &p_sys->bytestream );
                    p_sys->i_state = STATE_NOSYNC;
                    break;
                }
                /* Free bitrate only */
                if( p_sys->i_bit_rate == 0 )
                {
                    if( i_next_bit_rate != 0 )
                    {
                        p_sys->i_frame_size++;
                        break;
                    }
                }
            }
            else
            {
                /* Free bitrate only */
                if( p_sys->i_bit_rate == 0 )
                {
                    if( (unsigned int)p_sys->i_frame_size >
                        p_sys->i_max_frame_size )
                    {
                        msg_Dbg( p_dec, "frame too big %d > %d "
                                 "(emulated startcode ?)", p_sys->i_frame_size,
                                 p_sys->i_max_frame_size );
                        block_SkipByte( &p_sys->bytestream );
                        p_sys->i_state = STATE_NOSYNC;
                        p_sys->i_free_frame_size = MPGA_HEADER_SIZE;
                        break;
                    }
                    p_sys->i_frame_size++;
                    break;
                }
                msg_Dbg( p_dec, "emulated startcode "
                         "(no startcode on following frame)" );
                p_sys->i_state = STATE_NOSYNC;
                block_SkipByte( &p_sys->bytestream );
                break;
            }
            p_sys->i_state = STATE_SEND_DATA;
            break;
        case STATE_GET_DATA:
            /* Make sure we have enough data.
             * (Not useful if we went through NEXT_SYNC) */
            if( block_WaitBytes( &p_sys->bytestream,
                                 p_sys->i_frame_size ) != VLC_SUCCESS )
            {
                /* Need more data */
                return NULL;
            }
            p_sys->i_state = STATE_SEND_DATA;
        case STATE_SEND_DATA:
            if( !(p_buf = GetOutBuffer( p_dec, &p_out_buffer )) )
            {
                //p_dec->b_error = true;
                return NULL;
            }
            /* Free bitrate only */
            if( p_sys->i_bit_rate == 0 )
            {
                p_sys->i_free_frame_size = p_sys->i_frame_size;
            }
            /* Copy the whole frame into the buffer. */
            if (block_GetBytes( &p_sys->bytestream,
                            p_buf, __MIN( (unsigned)p_sys->i_frame_size, p_out_buffer->i_buffer ) )) {
                block_Release(p_out_buffer);
                return NULL;
            }
            /* Get beginning of next frame for libmad */
            if( !p_sys->b_packetizer )
            {
                assert( p_out_buffer->i_buffer >= (unsigned)p_sys->i_frame_size + MAD_BUFFER_GUARD );
                memcpy( p_buf + p_sys->i_frame_size,
                        p_header, MAD_BUFFER_GUARD );
            }
            p_sys->i_state = STATE_NOSYNC;
            /* Make sure we don't reuse the same pts twice */
            if( p_sys->i_pts == p_sys->bytestream.p_block->i_pts )
                p_sys->i_pts = p_sys->bytestream.p_block->i_pts = VLC_TS_INVALID;
            /* So p_block doesn't get re-added several times */
            p_block = block_BytestreamPop( &p_sys->bytestream );
            if (pp_block)
                *pp_block = p_block;
            else if (p_block)
                block_Release(p_block);
            return p_out_buffer;
        }
    }
    return NULL;
}
