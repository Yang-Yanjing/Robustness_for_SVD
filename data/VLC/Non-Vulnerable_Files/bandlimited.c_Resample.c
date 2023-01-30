 *****************************************************************************/
static block_t *Resample( filter_t * p_filter, block_t * p_in_buf )
{
    if( !p_in_buf || !p_in_buf->i_nb_samples )
    {
        if( p_in_buf )
            block_Release( p_in_buf );
        return NULL;
    }
    filter_sys_t *p_sys = p_filter->p_sys;
    unsigned int i_out_rate = p_filter->fmt_out.audio.i_rate;
    int i_nb_channels = aout_FormatNbChannels( &p_filter->fmt_in.audio );
    /* Check if we really need to run the resampler */
    if( i_out_rate == p_filter->fmt_in.audio.i_rate )
    {
        if( !(p_in_buf->i_flags & BLOCK_FLAG_DISCONTINUITY) &&
            p_sys->i_old_wing )
        {
            /* output the whole thing with the samples from last time */
            p_in_buf = block_Realloc( p_in_buf,
                p_sys->i_old_wing * p_filter->fmt_in.audio.i_bytes_per_frame,
                p_in_buf->i_buffer );
            if( !p_in_buf )
                return NULL;
            memcpy( p_in_buf->p_buffer, p_sys->p_buf +
                    i_nb_channels * p_sys->i_old_wing,
                    p_sys->i_old_wing *
                    p_filter->fmt_in.audio.i_bytes_per_frame );
            p_in_buf->i_nb_samples += p_sys->i_old_wing;
            p_in_buf->i_pts = date_Get( &p_sys->end_date );
            p_in_buf->i_length =
                date_Increment( &p_sys->end_date,
                                p_in_buf->i_nb_samples ) - p_in_buf->i_pts;
        }
        p_sys->i_old_wing = 0;
        p_sys->b_first = true;
        return p_in_buf;
    }
    unsigned i_bytes_per_frame = p_filter->fmt_out.audio.i_channels *
                                 p_filter->fmt_out.audio.i_bitspersample / 8;
    size_t i_out_size = i_bytes_per_frame * ( 1 + ( p_in_buf->i_nb_samples *
              p_filter->fmt_out.audio.i_rate / p_filter->fmt_in.audio.i_rate) )
            + p_filter->p_sys->i_buf_size;
    block_t *p_out_buf = block_Alloc( i_out_size );
    if( !p_out_buf )
    {
        block_Release( p_in_buf );
        return NULL;
    }
    if( (p_in_buf->i_flags & BLOCK_FLAG_DISCONTINUITY) || p_sys->b_first )
    {
        /* Continuity in sound samples has been broken, we'd better reset
         * everything. */
        p_out_buf->i_flags |= BLOCK_FLAG_DISCONTINUITY;
        p_sys->i_remainder = 0;
        date_Init( &p_sys->end_date, i_out_rate, 1 );
        date_Set( &p_sys->end_date, p_in_buf->i_pts );
        p_sys->d_old_factor = 1;
        p_sys->i_old_wing   = 0;
        p_sys->b_first = false;
    }
    size_t i_in_nb = p_in_buf->i_nb_samples;
    size_t i_in, i_out = 0;
    double d_factor, d_scale_factor, d_old_scale_factor;
    size_t i_filter_wing;
#if 0
    msg_Err( p_filter, "old rate: %i, old factor: %f, old wing: %i, i_in: %i",
             p_sys->i_old_rate, p_sys->d_old_factor,
             p_sys->i_old_wing, i_in_nb );
#endif
    /* Same format in and out... */
    assert( p_filter->fmt_in.audio.i_bytes_per_frame == i_bytes_per_frame );
    /* Prepare the source buffer */
    if( p_sys->i_old_wing )
    {   /* Copy all our samples in p_in_buf */
        /* Normally, there should be enough room for the old wing in the
         * buffer head room. Otherwise, we need to copy memory anyway. */
        p_in_buf = block_Realloc( p_in_buf,
                                  p_sys->i_old_wing * 2 * i_bytes_per_frame,
                                  p_in_buf->i_buffer );
        if( unlikely(p_in_buf == NULL) )
            return NULL;
        memcpy( p_in_buf->p_buffer, p_sys->p_buf,
                p_sys->i_old_wing * 2 * i_bytes_per_frame );
    }
    i_in_nb += (p_sys->i_old_wing * 2);
    float *p_in = (float *)p_in_buf->p_buffer;
    const float *p_in_orig = p_in;
    /* Make sure the output buffer is reset */
    memset( p_out_buf->p_buffer, 0, p_out_buf->i_buffer );
    /* Calculate the new length of the filter wing */
    d_factor = (double)i_out_rate / p_filter->fmt_in.audio.i_rate;
    i_filter_wing = ((SMALL_FILTER_NMULT+1)/2.0) * __MAX(1.0,1.0/d_factor) + 1;
    /* Account for increased filter gain when using factors less than 1 */
    d_old_scale_factor = SMALL_FILTER_SCALE *
        p_sys->d_old_factor + 0.5;
    d_scale_factor = SMALL_FILTER_SCALE * d_factor + 0.5;
    /* Apply the old rate until we have enough samples for the new one */
    i_in = p_sys->i_old_wing;
    p_in += p_sys->i_old_wing * i_nb_channels;
    size_t i_old_in_end = 0;
    if( p_sys->i_old_wing <= i_in_nb )
        i_old_in_end = __MIN( i_filter_wing, i_in_nb - p_sys->i_old_wing );
    ResampleFloat( p_filter,
                   &p_out_buf, &i_out, &p_in,
                   i_in, i_old_in_end,
                   p_sys->d_old_factor, true,
                   i_nb_channels, i_bytes_per_frame );
    i_in = __MAX( i_in, i_old_in_end );
    /* Apply the new rate for the rest of the samples */
    if( i_in < i_in_nb - i_filter_wing )
    {
        p_sys->d_old_factor = d_factor;
        p_sys->i_old_wing   = i_filter_wing;
    }
    if( p_out_buf )
    {
        ResampleFloat( p_filter,
                       &p_out_buf, &i_out, &p_in,
                       i_in, i_in_nb - i_filter_wing,
                       d_factor, false,
                       i_nb_channels, i_bytes_per_frame );
        /* Finalize aout buffer */
        p_out_buf->i_nb_samples = i_out;
        p_out_buf->i_dts =
        p_out_buf->i_pts = date_Get( &p_sys->end_date );
        p_out_buf->i_length = date_Increment( &p_sys->end_date,
                                      p_out_buf->i_nb_samples ) - p_out_buf->i_pts;
        p_out_buf->i_buffer = p_out_buf->i_nb_samples *
            i_nb_channels * sizeof(int32_t);
    }
    /* Buffer i_filter_wing * 2 samples for next time */
    if( p_sys->i_old_wing )
    {
        size_t newsize = p_sys->i_old_wing * 2 * i_bytes_per_frame;
        if( newsize > p_sys->i_buf_size )
        {
            free( p_sys->p_buf );
            p_sys->p_buf = malloc( newsize );
            if( p_sys->p_buf != NULL )
                p_sys->i_buf_size = newsize;
            else
            {
                p_sys->i_buf_size = p_sys->i_old_wing = 0; /* oops! */
                block_Release( p_in_buf );
                return p_out_buf;
            }
        }
        memcpy( p_sys->p_buf,
                p_in_orig + (i_in_nb - 2 * p_sys->i_old_wing) *
                i_nb_channels, (2 * p_sys->i_old_wing) *
                p_filter->fmt_in.audio.i_bytes_per_frame );
    }
#if 0
    msg_Err( p_filter, "p_out size: %i, nb bytes out: %i", p_out_buf->i_buffer,
             i_out * p_filter->fmt_in.audio.i_bytes_per_frame );
#endif
    block_Release( p_in_buf );
    return p_out_buf;
}
