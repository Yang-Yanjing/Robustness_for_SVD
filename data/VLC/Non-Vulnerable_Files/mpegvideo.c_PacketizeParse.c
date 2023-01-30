}
static block_t *PacketizeParse( void *p_private, bool *pb_ts_used, block_t *p_block )
{
    decoder_t *p_dec = p_private;
    /* Check if we have a picture start code */
    *pb_ts_used = p_block->p_buffer[3] == 0x00;
    return ParseMPEGBlock( p_dec, p_block );
}
