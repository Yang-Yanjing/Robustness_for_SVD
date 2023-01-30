 */
static void stereo2mono_downmix( filter_t * p_filter,
                                 block_t * p_in_buf, block_t * p_out_buf )
{
    filter_sys_t *p_sys = (filter_sys_t *)p_filter->p_sys;
    int i_input_nb = aout_FormatNbChannels( &p_filter->fmt_in.audio );
    int i_output_nb = aout_FormatNbChannels( &p_filter->fmt_out.audio );
    int16_t * p_in = (int16_t*) p_in_buf->p_buffer;
    uint8_t * p_out;
    uint8_t * p_overflow;
    uint8_t * p_slide;
    size_t i_overflow_size;     /* in bytes */
    size_t i_out_size;          /* in bytes */
    unsigned int i, j;
    int i_source_channel_offset;
    int i_dest_channel_offset;
    unsigned int i_delay;
    double d_amplitude_factor;
    /* out buffer characterisitcs */
    p_out_buf->i_nb_samples = p_in_buf->i_nb_samples;
    p_out_buf->i_buffer = p_in_buf->i_buffer * i_output_nb / i_input_nb;
    p_out = p_out_buf->p_buffer;
    i_out_size = p_out_buf->i_buffer;
    /* Slide the overflow buffer */
    p_overflow = p_sys->p_overflow_buffer;
    i_overflow_size = p_sys->i_overflow_buffer_size;
    if ( i_out_size > i_overflow_size )
        memcpy( p_out, p_overflow, i_overflow_size );
    else
        memcpy( p_out, p_overflow, i_out_size );
    p_slide = p_sys->p_overflow_buffer;
    while( p_slide < p_overflow + i_overflow_size )
    {
        if( p_slide + i_out_size < p_overflow + i_overflow_size )
        {
            memset( p_slide, 0, i_out_size );
            if( p_slide + 2 * i_out_size < p_overflow + i_overflow_size )
                memcpy( p_slide, p_slide + i_out_size, i_out_size );
            else
                memcpy( p_slide, p_slide + i_out_size,
                        p_overflow + i_overflow_size - ( p_slide + i_out_size ) );
        }
        else
        {
            memset( p_slide, 0, p_overflow + i_overflow_size - p_slide );
        }
        p_slide += i_out_size;
    }
    /* apply the atomic operations */
    for( i = 0; i < p_sys->i_nb_atomic_operations; i++ )
    {
        /* shorter variable names */
        i_source_channel_offset
            = p_sys->p_atomic_operations[i].i_source_channel_offset;
        i_dest_channel_offset
            = p_sys->p_atomic_operations[i].i_dest_channel_offset;
        i_delay = p_sys->p_atomic_operations[i].i_delay;
        d_amplitude_factor
            = p_sys->p_atomic_operations[i].d_amplitude_factor;
        if( p_out_buf->i_nb_samples > i_delay )
        {
            /* current buffer coefficients */
            for( j = 0; j < p_out_buf->i_nb_samples - i_delay; j++ )
            {
                ((int16_t*)p_out)[ (i_delay+j)*i_output_nb + i_dest_channel_offset ]
                    += p_in[ j * i_input_nb + i_source_channel_offset ]
                       * d_amplitude_factor;
            }
            /* overflow buffer coefficients */
            for( j = 0; j < i_delay; j++ )
            {
                ((int16_t*)p_overflow)[ j*i_output_nb + i_dest_channel_offset ]
                    += p_in[ (p_out_buf->i_nb_samples - i_delay + j)
                       * i_input_nb + i_source_channel_offset ]
                       * d_amplitude_factor;
            }
        }
        else
        {
            /* overflow buffer coefficients only */
            for( j = 0; j < p_out_buf->i_nb_samples; j++ )
            {
                ((int16_t*)p_overflow)[ (i_delay - p_out_buf->i_nb_samples + j)
                                        * i_output_nb + i_dest_channel_offset ]
                    += p_in[ j * i_input_nb + i_source_channel_offset ]
                       * d_amplitude_factor;
            }
        }
    }
}
