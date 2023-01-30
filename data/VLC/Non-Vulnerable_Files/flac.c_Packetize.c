/* */
static block_t *Packetize(decoder_t *p_dec, block_t **pp_block)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t p_header[MAX_FLAC_HEADER_SIZE];
    block_t *out, *in = NULL;
    if ( pp_block && *pp_block)
    {
        in = *pp_block;
        if (in->i_flags&(BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED)) {
            if (in->i_flags&BLOCK_FLAG_CORRUPTED) {
                p_sys->i_state = STATE_NOSYNC;
                block_BytestreamEmpty(&p_sys->bytestream);
            }
            block_Release(*pp_block);
            return NULL;
        }
    }
    if (!p_sys->b_stream_info)
        ProcessHeader(p_dec);
    if (p_sys->stream_info.channels > 8) {
        msg_Err(p_dec, "This stream uses too many audio channels (%d > 8)",
            p_sys->stream_info.channels);
        return NULL;
    }
    if ( p_sys->i_pts <= VLC_TS_INVALID )
    {
        if ( in && in->i_pts == p_sys->i_pts )
        {
            /* We've just started the stream, wait for the first PTS. */
            block_Release(in);
            return NULL;
        }
        p_sys->i_rate = p_dec->fmt_out.audio.i_rate;
    }
    if ( in )
        block_BytestreamPush(&p_sys->bytestream, in);
    while (1) switch (p_sys->i_state) {
    case STATE_NOSYNC:
        while (!block_PeekBytes(&p_sys->bytestream, p_header, 2)) {
            if (p_header[0] == 0xFF && (p_header[1] & 0xFE) == 0xF8) {
                p_sys->i_state = STATE_SYNC;
                break;
            }
            block_SkipByte(&p_sys->bytestream);
        }
        if (p_sys->i_state != STATE_SYNC) {
            block_BytestreamFlush(&p_sys->bytestream);
            return NULL; /* Need more data */
        }
    case STATE_SYNC:
        /* Sync state is unverified until we have read frame header and checked CRC
           Once validated, we'll send data from NEXT_SYNC state where we'll
           compute frame size */
        p_sys->i_state = STATE_HEADER;
    case STATE_HEADER:
        /* Get FLAC frame header (MAX_FLAC_HEADER_SIZE bytes) */
        if (block_PeekBytes(&p_sys->bytestream, p_header, MAX_FLAC_HEADER_SIZE))
            return NULL; /* Need more data */
        /* Check if frame is valid and get frame info */
        p_sys->i_frame_length = SyncInfo(p_dec, p_header,
                                          &p_sys->i_channels,
                                          &p_sys->i_rate,
                                          &p_sys->i_bits_per_sample,
                                          &p_sys->i_pts,
                                          &p_sys->i_duration );
        if (!p_sys->i_frame_length) {
            msg_Dbg(p_dec, "emulated sync word");
            block_SkipByte(&p_sys->bytestream);
            p_sys->i_state = STATE_NOSYNC;
            break;
        }
        if (p_sys->i_rate != p_dec->fmt_out.audio.i_rate) {
            p_dec->fmt_out.audio.i_rate = p_sys->i_rate;
        }
        p_sys->i_state = STATE_NEXT_SYNC;
        p_sys->i_frame_size = ( p_sys->b_stream_info ) ? p_sys->stream_info.min_framesize :
                                                         MIN_FLAC_FRAME_SIZE;
        /* We have to read until next frame sync code to compute current frame size
         * from that boundary.
         * The confusing part below is that sync code needs to be verified in case
         * it would appear in data, so we also need to check next frame header CRC
         */
    case STATE_NEXT_SYNC:
    {
        /* Calculate the initial CRC for the minimal frame size,
         * We'll update it as we look for the next start code. */
        uint8_t *buf = malloc(p_sys->i_frame_size);
        if (!buf)
            return NULL;
        if (block_PeekOffsetBytes(&p_sys->bytestream, 0, buf, p_sys->i_frame_size)) {
            free(buf);
            return NULL;
        }
        uint16_t crc = 0;
        for (unsigned i = 0; i < p_sys->i_frame_size; i++)
            crc = flac_crc16(crc, buf[i]);
        free(buf);
        p_sys->crc = crc;
        /* Check if next expected frame contains the sync word */
        while (!block_PeekOffsetBytes(&p_sys->bytestream, p_sys->i_frame_size,
                    p_header, MAX_FLAC_HEADER_SIZE)) {
            if (p_header[0] == 0xFF && (p_header[1] & 0xFE) == 0xF8) {
                /* Check if frame is valid and get frame info */
                int i_frame_length =
                    SyncInfo(p_dec, p_header,
                              &p_sys->i_channels,
                              &p_sys->i_rate,
                              &p_sys->i_bits_per_sample,
                              NULL, NULL );
                if (i_frame_length) {
                    uint8_t crc_bytes[2];
                    block_PeekOffsetBytes(&p_sys->bytestream,
                        p_sys->i_frame_size - 2, crc_bytes, 2);
                    /* Get the frame CRC */
                    uint16_t stream_crc = (crc_bytes[0] << 8) | crc_bytes[1];
                    /* Calculate the frame CRC: remove the last 2 bytes */
                    uint16_t crc = flac_crc16_undo(p_sys->crc, crc_bytes[1]);
                             crc = flac_crc16_undo(crc,        crc_bytes[0]);
                    if (stream_crc != crc) {
                        msg_Warn(p_dec, "Bad CRC for frame size %zu: 0x%x != 0x%x",
                            p_sys->i_frame_size, crc, stream_crc);
                        block_SkipByte(&p_sys->bytestream);
                        p_sys->i_state = STATE_NOSYNC;
                    } else {
                        p_sys->i_state = STATE_SEND_DATA;
                        p_sys->crc = 0;
                        break;
                    }
                }
            }
            p_sys->crc = flac_crc16(p_sys->crc, p_header[0]); /* update CRC */
            p_sys->i_frame_size++;
        }
        if (p_sys->i_state != STATE_SEND_DATA) {
            if (p_sys->b_stream_info && p_sys->stream_info.max_framesize > 0 &&
                p_sys->i_frame_size > p_sys->stream_info.max_framesize) {
                block_SkipByte(&p_sys->bytestream);
                msg_Warn(p_dec, "Frame is too big (%zu > %d), couldn't find start code",
                        p_sys->i_frame_size, p_sys->stream_info.max_framesize);
                p_sys->i_state = STATE_NOSYNC;
                return NULL;
            }
            if ( !in )
            {
                /* There's no following frame, so we need to read current
                 * data until the frame footer matches (crc16) == stream crc.
                 * In the worst case, if crc might be a false positive and data
                 * will be truncated. */
                uint8_t crc_bytes[2];
                if ( !block_PeekOffsetBytes(&p_sys->bytestream,
                                    p_sys->i_frame_size - 2, crc_bytes, 2) )
                {
                    while ( true )
                    {
                        /* Read the frame CRC */
                        uint16_t stream_crc = (crc_bytes[0] << 8) | crc_bytes[1];
                        /* Calculate the frame CRC: remove the last 2 bytes */
                        uint16_t crc = flac_crc16_undo(p_sys->crc, crc_bytes[1]);
                                 crc = flac_crc16_undo(crc,        crc_bytes[0]);
                        if (stream_crc == crc)
                        {
                            p_sys->i_state = STATE_SEND_DATA;
                            break;
                        }
                        p_sys->i_frame_size++;
                        if ( block_PeekOffsetBytes(&p_sys->bytestream,
                                                   p_sys->i_frame_size - 2, crc_bytes, 2) )
                            break;
                        /* Update current crc */
                        p_sys->crc = flac_crc16(p_sys->crc, crc_bytes[1]);
                    }
                }
                if ( p_sys->i_state != STATE_SEND_DATA )
                    return NULL;
            }
            else
            {
                /* Need more data */
                return NULL;
            }
        }
    }
    case STATE_SEND_DATA:
        out = block_Alloc(p_sys->i_frame_size);
        /* Copy the whole frame into the buffer. When we reach this point
         * we already know we have enough data available. */
        block_GetBytes(&p_sys->bytestream, out->p_buffer,
                        p_sys->i_frame_size);
        p_dec->fmt_out.audio.i_channels = p_sys->i_channels;
        p_dec->fmt_out.audio.i_physical_channels =
            p_dec->fmt_out.audio.i_original_channels =
                pi_channels_maps[p_sys->stream_info.channels];
        /* So p_block doesn't get re-added several times */
        if ( in )
            *pp_block = block_BytestreamPop(&p_sys->bytestream);
        else
            block_BytestreamFlush(&p_sys->bytestream);
        p_sys->i_state = STATE_NOSYNC;
        out->i_dts = out->i_pts = p_sys->i_pts;
        out->i_length = p_sys->i_duration;
        return out;
    }
    return NULL;
}
