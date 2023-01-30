}
static block_t *CreateAnnexbNAL( decoder_t *p_dec, const uint8_t *p, int i_size )
{
    block_t *p_nal;
    p_nal = block_Alloc( 4 + i_size );
    if( !p_nal ) return NULL;
    /* Add start code */
    p_nal->p_buffer[0] = 0x00;
    p_nal->p_buffer[1] = 0x00;
    p_nal->p_buffer[2] = 0x00;
    p_nal->p_buffer[3] = 0x01;
    /* Copy nalu */
    memcpy( &p_nal->p_buffer[4], p, i_size );
    VLC_UNUSED(p_dec);
    return p_nal;
}
