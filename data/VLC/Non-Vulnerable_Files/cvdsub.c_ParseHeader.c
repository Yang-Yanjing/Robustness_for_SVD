*/
static void ParseHeader( decoder_t *p_dec, block_t *p_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t *p = p_block->p_buffer;
    p_sys->i_spu_size = (p[0] << 8) + p[1] + 4; p += 2;
    /* FIXME: check data sanity */
    p_sys->metadata_offset = (p[0] <<  8) +   p[1]; p +=2;
    p_sys->metadata_length = p_sys->i_spu_size - p_sys->metadata_offset;
    p_sys->i_image_offset = 4;
    p_sys->i_image_length = p_sys->metadata_offset - p_sys->i_image_offset;
#ifdef DEBUG_CVDSUB
    msg_Dbg( p_dec, "total size: %zu  image size: %zu",
             p_sys->i_spu_size, p_sys->i_image_length );
#endif
}
