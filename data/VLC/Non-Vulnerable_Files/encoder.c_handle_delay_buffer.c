}
static block_t *handle_delay_buffer( encoder_t *p_enc, encoder_sys_t *p_sys, int buffer_delay, block_t *p_aout_buf, int leftover_samples )
{
    block_t *p_block = NULL;
    //How much we need to copy from new packet
    const int leftover = leftover_samples * p_sys->p_context->channels * p_sys->i_sample_bytes;
    avcodec_get_frame_defaults( p_sys->frame );
    p_sys->frame->format     = p_sys->p_context->sample_fmt;
    p_sys->frame->nb_samples = leftover_samples + p_sys->i_samples_delay;
    p_sys->frame->pts        = date_Get( &p_sys->buffer_date );
    if( likely( p_sys->frame->pts != AV_NOPTS_VALUE) )
        date_Increment( &p_sys->buffer_date, p_sys->frame->nb_samples );
    if( likely( p_aout_buf ) )
    {
        p_aout_buf->i_nb_samples -= leftover_samples;
        memcpy( p_sys->p_buffer+buffer_delay, p_aout_buf->p_buffer, leftover );
        // We need to deinterleave from p_aout_buf to p_buffer the leftover bytes
        if( p_sys->b_planar )
            aout_Deinterleave( p_sys->p_interleave_buf, p_sys->p_buffer,
                p_sys->i_frame_size, p_sys->p_context->channels, p_enc->fmt_in.i_codec );
        else
            memcpy( p_sys->p_buffer + buffer_delay, p_aout_buf->p_buffer, leftover);
        p_aout_buf->p_buffer     += leftover;
        p_aout_buf->i_buffer     -= leftover;
        if( likely( p_sys->frame->pts != AV_NOPTS_VALUE) )
            p_aout_buf->i_pts         = date_Get( &p_sys->buffer_date );
    }
    if(unlikely( ( (leftover + buffer_delay) < p_sys->i_buffer_out ) &&
                 !(p_sys->p_codec->capabilities & CODEC_CAP_SMALL_LAST_FRAME )))
    {
        msg_Dbg( p_enc, "No small last frame support, padding");
        size_t padding_size = p_sys->i_buffer_out - (leftover+buffer_delay);
        memset( p_sys->p_buffer + (leftover+buffer_delay), 0, padding_size );
        buffer_delay += padding_size;
    }
    if( avcodec_fill_audio_frame( p_sys->frame, p_sys->p_context->channels,
            p_sys->p_context->sample_fmt, p_sys->b_planar ? p_sys->p_interleave_buf : p_sys->p_buffer,
            p_sys->i_buffer_out,
            DEFAULT_ALIGN) < 0 )
    {
        msg_Err( p_enc, "filling error on fillup" );
        p_sys->frame->nb_samples = 0;
    }
    p_sys->i_samples_delay = 0;
    p_block = encode_audio_buffer( p_enc, p_sys, p_sys->frame );
    return p_block;
}
