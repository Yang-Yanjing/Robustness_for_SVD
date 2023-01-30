}
static block_t *Convert( filter_t *p_filter, block_t *p_block )
{
    if( !p_block || !p_block->i_nb_samples )
    {
        if( p_block )
            block_Release( p_block );
        return NULL;
    }
    size_t i_out_size = p_block->i_buffer *
        aout_FormatNbChannels( &(p_filter->fmt_out.audio) ) /
        aout_FormatNbChannels( &(p_filter->fmt_in.audio) );
    block_t *p_out = block_Alloc( i_out_size );
    if( !p_out )
    {
        msg_Warn( p_filter, "can't get output buffer" );
        block_Release( p_block );
        return NULL;
    }
    p_out->i_nb_samples = p_block->i_nb_samples;
    p_out->i_dts = p_block->i_dts;
    p_out->i_pts = p_block->i_pts;
    p_out->i_length = p_block->i_length;
    DoWork( p_filter, p_block, p_out );
    block_Release( p_block );
    return p_out;
}
