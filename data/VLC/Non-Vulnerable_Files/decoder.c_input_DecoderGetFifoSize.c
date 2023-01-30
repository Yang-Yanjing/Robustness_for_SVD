}
size_t input_DecoderGetFifoSize( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    return block_FifoSize( p_owner->p_fifo );
}
