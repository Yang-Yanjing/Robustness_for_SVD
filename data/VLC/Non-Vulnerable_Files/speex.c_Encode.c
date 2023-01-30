 ****************************************************************************/
static block_t *Encode( encoder_t *p_enc, block_t *p_aout_buf )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    block_t *p_block, *p_chain = NULL;
    /* Encoder gets NULL when it's time to flush */
    if( unlikely( !p_aout_buf ) ) return NULL;
    unsigned char *p_buffer = p_aout_buf->p_buffer;
    unsigned i_samples = p_aout_buf->i_nb_samples;
    int i_samples_delay = p_sys->i_samples_delay;
    mtime_t i_pts = p_aout_buf->i_pts -
                (mtime_t)1000000 * (mtime_t)p_sys->i_samples_delay /
                (mtime_t)p_enc->fmt_in.audio.i_rate;
    p_sys->i_samples_delay += i_samples;
    while( p_sys->i_samples_delay >= p_sys->i_frame_length )
    {
        int16_t *p_samples;
        int i_out;
        if( i_samples_delay )
        {
            /* Take care of the left-over from last time */
            int i_delay_size = i_samples_delay * 2 *
                                 p_enc->fmt_in.audio.i_channels;
            int i_size = p_sys->i_frame_size - i_delay_size;
            p_samples = (int16_t *)p_sys->p_buffer;
            memcpy( p_sys->p_buffer + i_delay_size, p_buffer, i_size );
            p_buffer -= i_delay_size;
            i_samples += i_samples_delay;
            i_samples_delay = 0;
        }
        else
        {
            p_samples = (int16_t *)p_buffer;
        }
        /* Encode current frame */
        if( p_enc->fmt_in.audio.i_channels == 2 )
            speex_encode_stereo_int( p_samples, p_sys->i_frame_length,
                                     &p_sys->bits );
#if 0
        if( p_sys->preprocess )
            speex_preprocess( p_sys->preprocess, p_samples, NULL );
#endif
        speex_encode_int( p_sys->p_state, p_samples, &p_sys->bits );
        p_buffer += p_sys->i_frame_size;
        p_sys->i_samples_delay -= p_sys->i_frame_length;
        i_samples -= p_sys->i_frame_length;
        p_sys->i_frames_in_packet++;
        if( p_sys->i_frames_in_packet < p_sys->header.frames_per_packet )
            continue;
        p_sys->i_frames_in_packet = 0;
        speex_bits_insert_terminator( &p_sys->bits );
        i_out = speex_bits_write( &p_sys->bits, p_sys->p_buffer_out,
                                  MAX_FRAME_BYTES );
        speex_bits_reset( &p_sys->bits );
        p_block = block_Alloc( i_out );
        memcpy( p_block->p_buffer, p_sys->p_buffer_out, i_out );
        p_block->i_length = (mtime_t)1000000 *
            (mtime_t)p_sys->i_frame_length * p_sys->header.frames_per_packet /
            (mtime_t)p_enc->fmt_in.audio.i_rate;
        p_block->i_dts = p_block->i_pts = i_pts;
        /* Update pts */
        i_pts += p_block->i_length;
        block_ChainAppend( &p_chain, p_block );
    }
    /* Backup the remaining raw samples */
    if( i_samples )
    {
        memcpy( p_sys->p_buffer + i_samples_delay * 2 *
                p_enc->fmt_in.audio.i_channels, p_buffer,
                i_samples * 2 * p_enc->fmt_in.audio.i_channels );
    }
    return p_chain;
}
