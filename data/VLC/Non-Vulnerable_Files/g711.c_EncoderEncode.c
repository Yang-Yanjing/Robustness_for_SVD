}
static block_t *EncoderEncode( encoder_t *p_enc, block_t *p_aout_buf )
{
    if( !p_aout_buf || !p_aout_buf->i_buffer ) return NULL;
    block_t *p_block = block_Alloc( p_aout_buf->i_buffer / 2 );
    if( unlikely(p_block == NULL ) )
        return NULL;
    const int16_t *src = (int16_t *)p_aout_buf->p_buffer; // source
    int8_t *dst = (int8_t *)p_block->p_buffer; // sink
    if( p_enc->fmt_out.i_codec == VLC_CODEC_ALAW )
    {
        for( unsigned i = 0; i < p_aout_buf->i_buffer / 2; i++ )
        {
             int16_t s = *(src++);
             if( s >= 0)
                 *(dst++) = alaw_encode[s / 16];
             else
                 *(dst++) = 0x7F & alaw_encode[s / -16];
        }
    }
    else /* µ-law */
    {
        for( unsigned i = 0; i < p_aout_buf->i_buffer / 2; i++ )
        {
             int16_t s = *(src++);
             if( s >= 0)
                 *(dst++) = ulaw_encode[s / 4];
             else
                 *(dst++) = 0x7F & ulaw_encode[s / -4];
        }
    }
    p_block->i_dts = p_block->i_pts = p_aout_buf->i_pts;
    p_block->i_length = (int64_t)p_aout_buf->i_nb_samples *
                        CLOCK_FREQ / p_enc->fmt_in.audio.i_rate;
    return p_block;
}
