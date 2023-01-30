 *****************************************************************************/
static block_t *Convert( filter_t *p_filter, block_t *p_block )
{
    block_t *p_out;
    int i_out_size;
    if( !p_block || !p_block->i_nb_samples )
    {
        if( p_block )
            block_Release( p_block );
        return NULL;
    }
    i_out_size = p_block->i_nb_samples * p_filter->p_sys->i_bitspersample/8 *
                 aout_FormatNbChannels( &(p_filter->fmt_out.audio) );
    p_out = block_Alloc( i_out_size );
    if( !p_out )
    {
        msg_Warn( p_filter, "can't get output buffer" );
        block_Release( p_block );
        return NULL;
    }
    p_out->i_nb_samples =
                  (p_block->i_nb_samples / p_filter->p_sys->i_nb_channels) *
                       aout_FormatNbChannels( &(p_filter->fmt_out.audio) );
#if 0
    unsigned int i_in_size = in_buf.i_nb_samples  * (p_filter->p_sys->i_bitspersample/8) *
                             aout_FormatNbChannels( &(p_filter->fmt_in.audio) );
    if( (in_buf.i_buffer != i_in_size) && ((i_in_size % 32) != 0) ) /* is it word aligned?? */
    {
        msg_Err( p_filter, "input buffer is not word aligned" );
        /* Fix output buffer to be word aligned */
    }
#endif
    memset( p_out->p_buffer, 0, i_out_size );
    if( p_filter->p_sys->b_downmix )
    {
        stereo2mono_downmix( p_filter, p_block, p_out );
        mono( p_filter, p_out, p_block );
    }
    else
    {
        stereo_to_mono( p_filter, p_out, p_block );
    }
    block_Release( p_block );
    return p_out;
}
