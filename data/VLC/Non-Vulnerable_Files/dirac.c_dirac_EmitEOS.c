 */
static block_t *dirac_EmitEOS( decoder_t *p_dec, uint32_t i_prev_parse_offset )
{
    const uint8_t p_eos[] = { 'B','B','C','D',0x10,0,0,0,13,0,0,0,0 };
    block_t *p_block = block_Alloc( 13 );
    if( !p_block )
        return NULL;
    memcpy( p_block->p_buffer, p_eos, 13 );
    SetDWBE( p_block->p_buffer + 9, i_prev_parse_offset );
    p_block->i_flags = DIRAC_NON_DATED;
    (void) p_dec;
    return p_block;
}
