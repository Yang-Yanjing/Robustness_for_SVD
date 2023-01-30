 *****************************************************************************/
static block_t *GetSoutBuffer( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block = block_Alloc( p_sys->frame.i_size );
    if( p_block )
    {
        p_block->i_pts = p_block->i_dts = date_Get( &p_sys->end_date );
        p_block->i_length =
            date_Increment( &p_sys->end_date, p_sys->frame.i_samples ) - p_block->i_pts;
    }
    return p_block;
}
