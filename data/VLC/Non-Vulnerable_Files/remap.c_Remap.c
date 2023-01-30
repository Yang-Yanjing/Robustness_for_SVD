 *****************************************************************************/
static block_t *Remap( filter_t *p_filter, block_t *p_block )
{
    filter_sys_t *p_sys = (filter_sys_t *)p_filter->p_sys;
    if( !p_block || !p_block->i_nb_samples )
    {
        if( p_block )
            block_Release( p_block );
        return NULL;
    }
    size_t i_out_size = p_block->i_nb_samples *
        p_filter->fmt_out.audio.i_bytes_per_frame;
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
    memset( p_out->p_buffer, 0, i_out_size );
    p_sys->pf_remap( p_filter,
                (const void *)p_block->p_buffer, (void *)p_out->p_buffer,
                p_block->i_nb_samples,
                p_filter->fmt_in.audio.i_channels,
                p_filter->fmt_out.audio.i_channels );
    block_Release( p_block );
    return p_out;
}
