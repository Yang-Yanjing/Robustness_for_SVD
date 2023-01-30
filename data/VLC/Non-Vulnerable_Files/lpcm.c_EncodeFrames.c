 *****************************************************************************/
static block_t *EncodeFrames( encoder_t *p_enc, block_t *p_aout_buf )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    block_t *p_first_block = NULL, *p_last_block = NULL;
    if( !p_aout_buf || !p_aout_buf->i_buffer ) return NULL;
    const int i_num_frames = ( p_sys->i_buffer_used + p_aout_buf->i_nb_samples ) /
        p_sys->i_frame_samples;
    const int i_leftover_samples = ( p_sys->i_buffer_used + p_aout_buf->i_nb_samples ) %
        p_sys->i_frame_samples;
    const int i_frame_size = p_sys->i_frame_samples * p_sys->i_channels * 2 + LPCM_VOB_HEADER_LEN;
    const int i_start_offset = -p_sys->i_buffer_used;
    uint8_t i_freq_code = 0;
    switch( p_sys->i_rate ) {
    case 48000:
        i_freq_code = 0;
        break;
    case 96000:
        i_freq_code = 1;
        break;
    case 44100:
        i_freq_code = 2;
        break;
    case 32000:
        i_freq_code = 3;
        break;
    default:
        assert(0);
    }
    int i_bytes_consumed = 0;
    for ( int i = 0; i < i_num_frames; ++i )
    {
        block_t *p_block = block_Alloc( i_frame_size );
        if( !p_block )
            return NULL;
        uint8_t *frame = (uint8_t *)p_block->p_buffer;
        frame[0] = 1;  /* one frame in packet */
        frame[1] = 0;
        frame[2] = 0;  /* no first access unit */
        frame[3] = (p_sys->i_frame_num + i) & 0x1f;  /* no emphasis, no mute */
        frame[4] = (i_freq_code << 4) | (p_sys->i_channels - 1);
        frame[5] = 0x80;  /* neutral dynamic range */
        const int i_consume_samples = p_sys->i_frame_samples - p_sys->i_buffer_used;
        const int i_kept_bytes = p_sys->i_buffer_used * p_sys->i_channels * 2;
        const int i_consume_bytes = i_consume_samples * p_sys->i_channels * 2;
#ifdef WORDS_BIGENDIAN
        memcpy( frame + 6, p_sys->p_buffer, i_kept_bytes );
        memcpy( frame + 6 + i_kept_bytes, p_aout_buf->p_buffer + i_bytes_consumed,
                i_consume_bytes );
#else
        swab( p_sys->p_buffer, frame + 6, i_kept_bytes );
        swab( p_aout_buf->p_buffer + i_bytes_consumed, frame + 6 + i_kept_bytes,
              i_consume_bytes );
#endif
        p_sys->i_frame_num++;
        p_sys->i_buffer_used = 0;
        i_bytes_consumed += i_consume_bytes;
        /* We need to find i_length by means of next_pts due to possible roundoff errors. */
        mtime_t this_pts = p_aout_buf->i_pts +
            (i * p_sys->i_frame_samples + i_start_offset) * CLOCK_FREQ / p_sys->i_rate;
        mtime_t next_pts = p_aout_buf->i_pts +
            ((i + 1) * p_sys->i_frame_samples + i_start_offset) * CLOCK_FREQ / p_sys->i_rate;
        p_block->i_pts = p_block->i_dts = this_pts;
        p_block->i_length = next_pts - this_pts;
        if( !p_first_block )
            p_first_block = p_last_block = p_block;
        else
            p_last_block = p_last_block->p_next = p_block;
    }
    memcpy( p_sys->p_buffer,
            p_aout_buf->p_buffer + i_bytes_consumed,
            i_leftover_samples * p_sys->i_channels * 2 );
    p_sys->i_buffer_used = i_leftover_samples;
    return p_first_block;
}
