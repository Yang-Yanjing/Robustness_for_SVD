 ****************************************************************************/
static block_t *EncodeAudio( encoder_t *p_enc, block_t *p_aout_buf )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    block_t *p_block, *p_chain = NULL;
    size_t buffer_delay = 0, i_samples_left = 0;
    //i_bytes_left is amount of bytes we get
    i_samples_left = p_aout_buf ? p_aout_buf->i_nb_samples : 0;
    buffer_delay = p_sys->i_samples_delay * p_sys->i_sample_bytes * p_sys->p_context->channels;
    //p_sys->i_buffer_out = p_sys->i_frame_size * chan * p_sys->i_sample_bytes
    //Calculate how many bytes we would need from current buffer to fill frame
    size_t leftover_samples = __MAX(0,__MIN((ssize_t)i_samples_left, (ssize_t)(p_sys->i_frame_size - p_sys->i_samples_delay)));
    if( p_aout_buf && ( p_aout_buf->i_pts > VLC_TS_INVALID ) )
    {
        date_Set( &p_sys->buffer_date, p_aout_buf->i_pts );
        /* take back amount we have leftover from previous buffer*/
        if( p_sys->i_samples_delay > 0 )
            date_Decrement( &p_sys->buffer_date, p_sys->i_samples_delay );
    }
    /* Handle reordering here so we have p_sys->p_buffer always in correct
     * order already */
    if( p_aout_buf && p_sys->i_channels_to_reorder > 0 )
    {
        aout_ChannelReorder( p_aout_buf->p_buffer, p_aout_buf->i_buffer,
             p_sys->i_channels_to_reorder, p_sys->pi_reorder_layout,
             p_enc->fmt_in.i_codec );
    }
    // Check if we have enough samples in delay_buffer and current p_aout_buf to fill frame
    // Or if we are cleaning up
    if( ( buffer_delay > 0 ) &&
            ( ( p_aout_buf && ( leftover_samples <= p_aout_buf->i_nb_samples ) &&
               ( (leftover_samples + p_sys->i_samples_delay ) >= p_sys->i_frame_size )
              ) ||
             ( !p_aout_buf )
            )
         )
    {
        p_chain = handle_delay_buffer( p_enc, p_sys, buffer_delay, p_aout_buf, leftover_samples );
        buffer_delay = 0;
        if( unlikely( !p_chain ) )
            return NULL;
    }
    if( unlikely( !p_aout_buf ) )
    {
        msg_Dbg(p_enc,"Flushing..");
        do {
            p_block = encode_audio_buffer( p_enc, p_sys, NULL );
            if( likely( p_block ) )
            {
                block_ChainAppend( &p_chain, p_block );
            }
        } while( p_block );
        return p_chain;
    }
    while( ( p_aout_buf->i_nb_samples >= p_sys->i_frame_size ) ||
           ( p_sys->b_variable && p_aout_buf->i_nb_samples ) )
    {
        avcodec_get_frame_defaults( p_sys->frame );
        if( p_sys->b_variable )
            p_sys->frame->nb_samples = p_aout_buf->i_nb_samples;
        else
            p_sys->frame->nb_samples = p_sys->i_frame_size;
        p_sys->frame->format     = p_sys->p_context->sample_fmt;
        p_sys->frame->pts        = date_Get( &p_sys->buffer_date );
        const int in_bytes = p_sys->frame->nb_samples *
            p_sys->p_context->channels * p_sys->i_sample_bytes;
        if( p_sys->b_planar )
        {
            aout_Deinterleave( p_sys->p_buffer, p_aout_buf->p_buffer,
                               p_sys->frame->nb_samples, p_sys->p_context->channels, p_enc->fmt_in.i_codec );
        }
        else
        {
            memcpy(p_sys->p_buffer, p_aout_buf->p_buffer, in_bytes);
        }
        if( avcodec_fill_audio_frame( p_sys->frame, p_sys->p_context->channels,
                                    p_sys->p_context->sample_fmt,
                                    p_sys->p_buffer,
                                    p_sys->i_buffer_out,
                                    DEFAULT_ALIGN) < 0 )
        {
                 msg_Err( p_enc, "filling error on encode" );
                 p_sys->frame->nb_samples = 0;
        }
        p_aout_buf->p_buffer     += in_bytes;
        p_aout_buf->i_buffer     -= in_bytes;
        p_aout_buf->i_nb_samples -= p_sys->frame->nb_samples;
        if( likely( p_sys->frame->pts != AV_NOPTS_VALUE) )
            date_Increment( &p_sys->buffer_date, p_sys->frame->nb_samples );
        p_block = encode_audio_buffer( p_enc, p_sys, p_sys->frame );
        if( likely( p_block ) )
            block_ChainAppend( &p_chain, p_block );
    }
    // We have leftover samples that don't fill frame_size, and libavcodec doesn't seem to like
    // that frame has more data than p_sys->i_frame_size most of the cases currently.
    if( p_aout_buf->i_nb_samples > 0 )
    {
       memcpy( p_sys->p_buffer + buffer_delay, p_aout_buf->p_buffer,
               p_aout_buf->i_nb_samples * p_sys->i_sample_bytes * p_sys->p_context->channels);
       p_sys->i_samples_delay += p_aout_buf->i_nb_samples;
    }
    return p_chain;
}
