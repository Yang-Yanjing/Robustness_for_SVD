 ****************************************************************************/
static block_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t p_header[VLC_A52_HEADER_SIZE];
    uint8_t *p_buf;
    block_t *p_out_buffer;
    if( !pp_block || !*pp_block ) return NULL;
    if( (*pp_block)->i_flags&(BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
    {
        if( (*pp_block)->i_flags&BLOCK_FLAG_CORRUPTED )
        {
            p_sys->i_state = STATE_NOSYNC;
            block_BytestreamEmpty( &p_sys->bytestream );
        }
        date_Set( &p_sys->end_date, 0 );
        block_Release( *pp_block );
        return NULL;
    }
    if( !date_Get( &p_sys->end_date ) && (*pp_block)->i_pts <= VLC_TS_INVALID)
    {
        /* We've just started the stream, wait for the first PTS. */
        block_Release( *pp_block );
        return NULL;
    }
    block_BytestreamPush( &p_sys->bytestream, *pp_block );
    while( 1 )
    {
        switch( p_sys->i_state )
        {
        case STATE_NOSYNC:
            while( block_PeekBytes( &p_sys->bytestream, p_header, 2 )
                   == VLC_SUCCESS )
            {
                if( p_header[0] == 0x0b && p_header[1] == 0x77 )
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
                p_sys->i_pts != date_Get( &p_sys->end_date ) )
            {
                date_Set( &p_sys->end_date, p_sys->i_pts );
            }
            p_sys->i_state = STATE_HEADER;
        case STATE_HEADER:
            /* Get A/52 frame header (VLC_A52_HEADER_SIZE bytes) */
            if( block_PeekBytes( &p_sys->bytestream, p_header,
                                 VLC_A52_HEADER_SIZE ) != VLC_SUCCESS )
            {
                /* Need more data */
                return NULL;
            }
            /* Check if frame is valid and get frame info */
            if( vlc_a52_header_Parse( &p_sys->frame, p_header, VLC_A52_HEADER_SIZE ) )
            {
                msg_Dbg( p_dec, "emulated sync word" );
                block_SkipByte( &p_sys->bytestream );
                p_sys->i_state = STATE_NOSYNC;
                break;
            }
            p_sys->i_state = STATE_NEXT_SYNC;
        case STATE_NEXT_SYNC:
            /* TODO: If pp_block == NULL, flush the buffer without checking the
             * next sync word */
            /* Check if next expected frame contains the sync word */
            if( block_PeekOffsetBytes( &p_sys->bytestream,
                                       p_sys->frame.i_size, p_header, 2 )
                != VLC_SUCCESS )
            {
                /* Need more data */
                return NULL;
            }
            if( p_sys->b_packetizer &&
                p_header[0] == 0 && p_header[1] == 0 )
            {
                /* A52 wav files and audio CD's use stuffing */
                p_sys->i_state = STATE_GET_DATA;
                break;
            }
            if( p_header[0] != 0x0b || p_header[1] != 0x77 )
            {
                msg_Dbg( p_dec, "emulated sync word "
                         "(no sync on following frame)" );
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
                                 p_sys->frame.i_size ) != VLC_SUCCESS )
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
            /* Copy the whole frame into the buffer. When we reach this point
             * we already know we have enough data available. */
            block_GetBytes( &p_sys->bytestream,
                            p_buf, __MIN( p_sys->frame.i_size, p_out_buffer->i_buffer ) );
            /* Make sure we don't reuse the same pts twice */
            if( p_sys->i_pts == p_sys->bytestream.p_block->i_pts )
                p_sys->i_pts = p_sys->bytestream.p_block->i_pts = VLC_TS_INVALID;
            /* So p_block doesn't get re-added several times */
            *pp_block = block_BytestreamPop( &p_sys->bytestream );
            p_sys->i_state = STATE_NOSYNC;
            return p_out_buffer;
        }
    }
}
