 *****************************************************************************/
static block_t *Convert( filter_t *p_filter, block_t *p_in_buf )
{
    filter_sys_t    *p_sys = p_filter->p_sys;
    sample_t        i_sample_level = 1;
    int             i_flags = p_sys->i_flags;
    size_t          i_bytes_per_block = 256 * p_sys->i_nb_channels
                      * sizeof(float);
    block_t *p_out_buf = block_Alloc( 6 * i_bytes_per_block );
    if( unlikely(p_out_buf == NULL) )
        goto out;
    /*
     * Do the actual decoding now.
     */
    /* Needs to be called so the decoder knows which type of bitstream it is
     * dealing with. */
    int i_sample_rate, i_bit_rate, i_frame_length;
    if( !dca_syncinfo( p_sys->p_libdca, p_in_buf->p_buffer, &i_flags,
                       &i_sample_rate, &i_bit_rate, &i_frame_length ) )
    {
        msg_Warn( p_filter, "libdca couldn't sync on frame" );
        p_out_buf->i_nb_samples = p_out_buf->i_buffer = 0;
        goto out;
    }
    i_flags = p_sys->i_flags;
    dca_frame( p_sys->p_libdca, p_in_buf->p_buffer,
               &i_flags, &i_sample_level, 0 );
    if ( (i_flags & DCA_CHANNEL_MASK) != (p_sys->i_flags & DCA_CHANNEL_MASK)
          && !p_sys->b_dontwarn )
    {
        msg_Warn( p_filter,
                  "libdca couldn't do the requested downmix 0x%x->0x%x",
                  p_sys->i_flags  & DCA_CHANNEL_MASK,
                  i_flags & DCA_CHANNEL_MASK );
        p_sys->b_dontwarn = 1;
    }
    if( 0)//!p_sys->b_dynrng )
    {
        dca_dynrng( p_sys->p_libdca, NULL, NULL );
    }
    for( int i = 0; i < dca_blocks_num(p_sys->p_libdca); i++ )
    {
        sample_t * p_samples;
        if( dca_block( p_sys->p_libdca ) )
        {
            msg_Warn( p_filter, "dca_block failed for block %d", i );
            break;
        }
        p_samples = dca_samples( p_sys->p_libdca );
        if ( (p_sys->i_flags & DCA_CHANNEL_MASK) == DCA_MONO
              && (p_filter->fmt_out.audio.i_physical_channels
                   & (AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT)) )
        {
            Duplicate( (float *)(p_out_buf->p_buffer + i * i_bytes_per_block),
                       p_samples );
        }
        else if ( p_filter->fmt_out.audio.i_original_channels
                    & AOUT_CHAN_REVERSESTEREO )
        {
            Exchange( (float *)(p_out_buf->p_buffer + i * i_bytes_per_block),
                      p_samples );
        }
        else
        {
            /* Interleave the *$£%ù samples. */
            Interleave( (float *)(p_out_buf->p_buffer + i * i_bytes_per_block),
                        p_samples, p_sys->i_nb_channels, p_sys->pi_chan_table);
        }
    }
    p_out_buf->i_buffer = p_in_buf->i_nb_samples * 4 * p_sys->i_nb_channels;
    p_out_buf->i_nb_samples = p_in_buf->i_nb_samples;
    p_out_buf->i_dts = p_in_buf->i_dts;
    p_out_buf->i_pts = p_in_buf->i_pts;
    p_out_buf->i_length = p_in_buf->i_length;
out:
    block_Release( p_in_buf );
    return p_out_buf;
}
