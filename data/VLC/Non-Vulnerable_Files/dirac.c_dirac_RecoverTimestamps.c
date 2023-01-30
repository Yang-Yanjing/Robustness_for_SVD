 */
static void dirac_RecoverTimestamps ( decoder_t *p_dec, size_t i_length )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block = p_sys->bytestream.p_block;
    /* Find the block with first non-flushed data */
    size_t i_offset = p_sys->bytestream.i_offset;
    for(; p_block != NULL; p_block = p_block->p_next )
    {
        if( i_offset < p_block->i_buffer )
            break;
        i_offset -= p_block->i_buffer;
    }
    i_offset += i_length;
    for(; p_block != NULL; p_block = p_block->p_next )
    {
        if( p_sys->i_sync_pts <= VLC_TS_INVALID && p_sys->i_sync_dts <= VLC_TS_INVALID )
        {
            /* oldest timestamp wins */
            p_sys->i_sync_pts = p_block->i_pts;
            p_sys->i_sync_dts = p_block->i_dts;
        }
        /* clear timestamps -- more than one data unit can come from a block */
        p_block->i_flags = 0;
        p_block->i_pts = p_block->i_dts = VLC_TS_INVALID;
        if( i_offset < p_block->i_buffer )
            break;
        i_offset -= p_block->i_buffer;
    }
}
