 ****************************************************************************/
static block_t *Packetize( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t p_header[MLP_HEADER_SIZE];
    block_t *p_out_buffer;
    /* */
    if( !pp_block || !*pp_block )
        return NULL;
    /* */
    if( (*pp_block)->i_flags&(BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
    {
        if( (*pp_block)->i_flags&BLOCK_FLAG_CORRUPTED )
        {
            p_sys->b_mlp = false;
            p_sys->i_state = STATE_NOSYNC;
            block_BytestreamEmpty( &p_sys->bytestream );
        }
        date_Set( &p_sys->end_date, 0 );
        block_Release( *pp_block );
        return NULL;
    }
    if( !date_Get( &p_sys->end_date ) && !(*pp_block)->i_pts )
    {
        /* We've just started the stream, wait for the first PTS. */
        block_Release( *pp_block );
        return NULL;
    }
    block_BytestreamPush( &p_sys->bytestream, *pp_block );
    for( ;; )
    {
        switch( p_sys->i_state )
        {
        case STATE_NOSYNC:
            while( !block_PeekBytes( &p_sys->bytestream, p_header, MLP_HEADER_SIZE ) )
            {
                if( SyncInfo( p_header, &p_sys->b_mlp, &p_sys->mlp ) > 0 )
                {
                    p_sys->i_state = STATE_SYNC;
                    break;
                }
                else if( SyncInfoDolby( p_header ) > 0 )
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
            /* Get a MLP header */
            if( block_PeekBytes( &p_sys->bytestream, p_header, MLP_HEADER_SIZE ) )
            {
                /* Need more data */
                return NULL;
            }
            /* Check if frame is valid and get frame info */
            p_sys->i_frame_size = SyncInfoDolby( p_header );
            if( p_sys->i_frame_size <= 0 )
                p_sys->i_frame_size = SyncInfo( p_header, &p_sys->b_mlp, &p_sys->mlp );
            if( p_sys->i_frame_size <= 0 )
            {
                msg_Dbg( p_dec, "emulated sync word" );
                block_SkipByte( &p_sys->bytestream );
                p_sys->b_mlp = false;
                p_sys->i_state = STATE_NOSYNC;
                break;
            }
            p_sys->i_state = STATE_NEXT_SYNC;
        case STATE_NEXT_SYNC:
            /* TODO: If pp_block == NULL, flush the buffer without checking the
             * next sync word */
            /* Check if next expected frame contains the sync word */
            if( block_PeekOffsetBytes( &p_sys->bytestream,
                                       p_sys->i_frame_size, p_header, MLP_HEADER_SIZE ) )
            {
                /* Need more data */
                return NULL;
            }
            bool b_mlp = p_sys->b_mlp;
            mlp_header_t mlp = p_sys->mlp;
            if( SyncInfo( p_header, &b_mlp, &mlp ) <= 0 && SyncInfoDolby( p_header ) <= 0 )
            {
                msg_Dbg( p_dec, "emulated sync word "
                         "(no sync on following frame)" );
                p_sys->b_mlp = false;
                p_sys->i_state = STATE_NOSYNC;
                block_SkipByte( &p_sys->bytestream );
                break;
            }
            p_sys->i_state = STATE_SEND_DATA;
            break;
        case STATE_GET_DATA:
            /* Make sure we have enough data.
             * (Not useful if we went through NEXT_SYNC) */
            if( block_WaitBytes( &p_sys->bytestream, p_sys->i_frame_size ) )
            {
                /* Need more data */
                return NULL;
            }
            p_sys->i_state = STATE_SEND_DATA;
        case STATE_SEND_DATA:
            /* When we reach this point we already know we have enough
             * data available. */
            p_out_buffer = block_Alloc( p_sys->i_frame_size );
            if( !p_out_buffer )
                return NULL;
            /* Copy the whole frame into the buffer */
            block_GetBytes( &p_sys->bytestream,
                            p_out_buffer->p_buffer, p_out_buffer->i_buffer );
            /* Just ignore (E)AC3 frames */
            if( SyncInfoDolby( p_out_buffer->p_buffer ) > 0 )
            {
                block_Release( p_out_buffer );
                p_sys->i_state = STATE_NOSYNC;
                break;
            }
            /* Setup output */
            if( p_dec->fmt_out.audio.i_rate != p_sys->mlp.i_rate )
            {
                msg_Info( p_dec, "MLP channels: %d samplerate: %d",
                          p_sys->mlp.i_channels, p_sys->mlp.i_rate );
                if( p_sys->mlp.i_rate > 0 )
                {
                    const mtime_t i_end_date = date_Get( &p_sys->end_date );
                    date_Init( &p_sys->end_date, p_sys->mlp.i_rate, 1 );
                    date_Set( &p_sys->end_date, i_end_date );
                }
            }
            p_dec->fmt_out.audio.i_rate     = p_sys->mlp.i_rate;
            p_dec->fmt_out.audio.i_channels = p_sys->mlp.i_channels;
            p_dec->fmt_out.audio.i_original_channels = p_sys->mlp.i_channels_conf;
            p_dec->fmt_out.audio.i_physical_channels = p_sys->mlp.i_channels_conf;
            p_out_buffer->i_pts = p_out_buffer->i_dts = date_Get( &p_sys->end_date );
            p_out_buffer->i_length =
                date_Increment( &p_sys->end_date, p_sys->mlp.i_samples ) - p_out_buffer->i_pts;
            /* Make sure we don't reuse the same pts twice */
            if( p_sys->i_pts == p_sys->bytestream.p_block->i_pts )
                p_sys->i_pts = p_sys->bytestream.p_block->i_pts = VLC_TS_INVALID;
            /* So p_block doesn't get re-added several times */
            *pp_block = block_BytestreamPop( &p_sys->bytestream );
            p_sys->i_state = STATE_NOSYNC;
            return p_out_buffer;
        }
    }
    return NULL;
}
