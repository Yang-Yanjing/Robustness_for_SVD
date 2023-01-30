 */
static block_t *DoWork( filter_t *p_filter, block_t *p_in_buf )
{
    struct filter_sys_t *p_sys = p_filter->p_sys;
    int i_chan;
    unsigned i_samples = p_in_buf->i_nb_samples; /* number of samples */
    /* maximum number of samples to offset in buffer */
    int i_maxOffset = floor( p_sys->f_sweepDepth * p_sys->i_sampleRate / 1000 );
    float *p_out = (float*)p_in_buf->p_buffer;
    float *p_in =  (float*)p_in_buf->p_buffer;
    float *p_ptr, f_temp = 0;/* f_diff = 0, f_frac = 0;*/
    /* Process each sample */
    for( unsigned i = 0; i < i_samples ; i++ )
    {
        /* Sine function as a oscillator wave to calculate sweep */
        p_sys->i_cumulative += p_sys->i_step;
        p_sys->f_offset = sinf( (p_sys->i_cumulative) * p_sys->f_sinMultiplier )
                * floorf(p_sys->f_sweepDepth * p_sys->i_sampleRate / 1000);
        if( abs( p_sys->i_step ) > 0 )
        {
            if( p_sys->i_cumulative >=  floor( p_sys->f_sweepDepth *
                        p_sys->i_sampleRate / p_sys->f_sweepRate ))
            {
                p_sys->f_offset = i_maxOffset;
                p_sys->i_step = -1 * ( p_sys->i_step );
            }
            if( p_sys->i_cumulative <= floor( -1 * p_sys->f_sweepDepth *
                        p_sys->i_sampleRate / p_sys->f_sweepRate ) )
            {
                p_sys->f_offset = -i_maxOffset;
                p_sys->i_step = -1 * ( p_sys->i_step );
            }
        }
        /* Calculate position in delay */
        int offset = floor( p_sys->f_offset );
        p_ptr = p_sys->p_write + ( i_maxOffset - offset ) * p_sys->i_channels;
        /* Handle Overflow */
        if( p_ptr < p_sys->p_delayLineStart )
        {
            p_ptr += p_sys->i_bufferLength - p_sys->i_channels;
        }
        if( p_ptr > p_sys->p_delayLineEnd - 2*p_sys->i_channels )
        {
            p_ptr -= p_sys->i_bufferLength - p_sys->i_channels;
        }
        /* For interpolation */
/*        f_frac = ( p_sys->f_offset - (int)p_sys->f_offset );*/
        for( i_chan = 0; i_chan < p_sys->i_channels; i_chan++ )
        {
/*            if( p_ptr <= p_sys->p_delayLineStart + p_sys->i_channels )
                f_diff = *(p_sys->p_delayLineEnd + i_chan) - p_ptr[i_chan];
            else
                f_diff = *( p_ptr - p_sys->i_channels + i_chan )
                            - p_ptr[i_chan];*/
            f_temp = ( *( p_ptr + i_chan ) );//+ f_diff * f_frac;
            /*Linear Interpolation. FIXME. This creates LOTS of noise */
            sanitize(&f_temp);
            p_out[i_chan] = p_sys->f_dryLevel * p_in[i_chan] +
                p_sys->f_wetLevel * f_temp;
            *( p_sys->p_write + i_chan ) = p_in[i_chan] +
                p_sys->f_feedbackGain * f_temp;
        }
        if( p_sys->p_write == p_sys->p_delayLineStart )
            for( i_chan = 0; i_chan < p_sys->i_channels; i_chan++ )
                *( p_sys->p_delayLineEnd - p_sys->i_channels + i_chan )
                    = *( p_sys->p_delayLineStart + i_chan );
        p_in += p_sys->i_channels;
        p_out += p_sys->i_channels;
        p_sys->p_write += p_sys->i_channels;
        if( p_sys->p_write == p_sys->p_delayLineEnd - p_sys->i_channels )
        {
            p_sys->p_write = p_sys->p_delayLineStart;
        }
    }
    return p_in_buf;
}
