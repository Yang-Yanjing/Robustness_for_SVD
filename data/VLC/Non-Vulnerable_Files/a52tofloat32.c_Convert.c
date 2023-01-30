 *****************************************************************************/
static block_t *Convert( filter_t *p_filter, block_t *p_in_buf )
{
    filter_sys_t *p_sys = p_filter->p_sys;
#ifdef LIBA52_FIXED
    sample_t i_sample_level = (1 << 24);
#else
    sample_t i_sample_level = 1;
#endif
    int i_flags = p_sys->i_flags;
    size_t i_bytes_per_block = 256 * p_sys->i_nb_channels * sizeof(sample_t);
    block_t *p_out_buf = block_Alloc( 6 * i_bytes_per_block );
    if( unlikely(p_out_buf == NULL) )
        goto out;
    /* Do the actual decoding now. */
    a52_frame( p_sys->p_liba52, p_in_buf->p_buffer,
               &i_flags, &i_sample_level, 0 );
    if ( (i_flags & A52_CHANNEL_MASK) != (p_sys->i_flags & A52_CHANNEL_MASK)
          && !p_sys->b_dontwarn )
    {
        msg_Warn( p_filter,
                  "liba52 couldn't do the requested downmix 0x%x->0x%x",
                  p_sys->i_flags  & A52_CHANNEL_MASK,
                  i_flags & A52_CHANNEL_MASK );
        p_sys->b_dontwarn = 1;
    }
    if( !p_sys->b_dynrng )
    {
        a52_dynrng( p_sys->p_liba52, NULL, NULL );
    }
    for( unsigned i = 0; i < 6; i++ )
    {
        sample_t * p_samples;
        if( a52_block( p_sys->p_liba52 ) )
        {
            msg_Warn( p_filter, "a52_block failed for block %d", i );
        }
        p_samples = a52_samples( p_sys->p_liba52 );
        if ( ((p_sys->i_flags & A52_CHANNEL_MASK) == A52_CHANNEL1
               || (p_sys->i_flags & A52_CHANNEL_MASK) == A52_CHANNEL2
               || (p_sys->i_flags & A52_CHANNEL_MASK) == A52_MONO)
              && (p_filter->fmt_out.audio.i_physical_channels
                   & (AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT)) )
        {
            Duplicate( (sample_t *)(p_out_buf->p_buffer + i * i_bytes_per_block),
                       p_samples );
        }
        else if ( p_filter->fmt_out.audio.i_original_channels
                    & AOUT_CHAN_REVERSESTEREO )
        {
            Exchange( (sample_t *)(p_out_buf->p_buffer + i * i_bytes_per_block),
                      p_samples );
        }
        else
        {
            /* Interleave the *$£%ù samples. */
            Interleave( (sample_t *)(p_out_buf->p_buffer + i * i_bytes_per_block),
                        p_samples, p_sys->i_nb_channels, p_sys->pi_chan_table);
        }
    }
    p_out_buf->i_nb_samples = p_in_buf->i_nb_samples;
    p_out_buf->i_dts = p_in_buf->i_dts;
    p_out_buf->i_pts = p_in_buf->i_pts;
    p_out_buf->i_length = p_in_buf->i_length;
out:
    block_Release( p_in_buf );
    return p_out_buf;
}
