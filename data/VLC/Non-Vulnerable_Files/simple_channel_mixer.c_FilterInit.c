 *****************************************************************************/
static bool FilterInit( filter_t *p_filter, block_t *p_block, block_t **pp_out )
{
    if( !p_block || !p_block->i_nb_samples )
    {
        if( p_block )
            block_Release( p_block );
        return false;
    }
    size_t i_out_size = p_block->i_nb_samples *
        p_filter->fmt_out.audio.i_bitspersample *
        p_filter->fmt_out.audio.i_channels / 8;
    block_t *p_out = block_Alloc( i_out_size );
    if( !p_out )
    {
        msg_Warn( p_filter, "can't get output buffer" );
        block_Release( p_block );
        return false;
    }
    p_out->i_nb_samples = p_block->i_nb_samples;
    p_out->i_dts = p_block->i_dts;
    p_out->i_pts = p_block->i_pts;
    p_out->i_length = p_block->i_length;
    int i_input_nb = aout_FormatNbChannels( &p_filter->fmt_in.audio );
    int i_output_nb = aout_FormatNbChannels( &p_filter->fmt_out.audio );
    p_out->i_buffer = p_block->i_buffer * i_output_nb / i_input_nb;
    *pp_out = p_out;
    return true;
}
