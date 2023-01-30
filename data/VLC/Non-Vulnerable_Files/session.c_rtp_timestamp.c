}
static inline uint32_t rtp_timestamp (const block_t *block)
{
    assert (block->i_buffer >= 12);
    return GetDWBE (block->p_buffer + 4);
}
