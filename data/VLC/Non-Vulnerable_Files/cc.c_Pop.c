}
static block_t *Pop( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    int i_index;
    /* XXX Cc captions data are OUT OF ORDER (because we receive them in the bitstream
     * order (ie ordered by video picture dts) instead of the display order.
     *  We will simulate a simple IPB buffer scheme
     * and reorder with pts.
     * XXX it won't work with H264 which use non out of order B picture or MMCO
     */
    /* Wait for a P and output all *previous* picture by pts order (for
     * hierarchical B frames) */
    if( p_sys->i_block <= 1 ||
        ( p_sys->pp_block[p_sys->i_block-1]->i_flags & BLOCK_FLAG_TYPE_B ) )
        return NULL;
    p_block = p_sys->pp_block[i_index = 0];
    if( p_block->i_pts > VLC_TS_INVALID )
    {
        for( int i = 1; i < p_sys->i_block-1; i++ )
        {
            if( p_sys->pp_block[i]->i_pts > VLC_TS_INVALID && p_block->i_pts > VLC_TS_INVALID &&
                p_sys->pp_block[i]->i_pts < p_block->i_pts )
                p_block = p_sys->pp_block[i_index = i];
        }
    }
    assert( i_index+1 < p_sys->i_block );
    memmove( &p_sys->pp_block[i_index], &p_sys->pp_block[i_index+1], sizeof(*p_sys->pp_block) * ( p_sys->i_block - i_index - 1 ) );
    p_sys->i_block--;
    return p_block;
}
