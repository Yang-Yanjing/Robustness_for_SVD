}
static inline uint16_t rtp_seq (const block_t *block)
{
    assert (block->i_buffer >= 4);
    return GetWBE (block->p_buffer + 2);
}
