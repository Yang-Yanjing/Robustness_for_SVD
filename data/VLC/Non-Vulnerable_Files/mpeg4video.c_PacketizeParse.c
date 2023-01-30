}
static block_t *PacketizeParse( void *p_private, bool *pb_ts_used, block_t *p_block )
{
    decoder_t *p_dec = p_private;
    const mtime_t i_dts = p_block->i_dts;
    const mtime_t i_pts = p_block->i_pts;
    block_t *p_au = ParseMPEGBlock( p_dec, p_block );
    *pb_ts_used = p_au &&  p_au->i_dts == i_dts && p_au->i_pts == i_pts;
    return p_au;
}
