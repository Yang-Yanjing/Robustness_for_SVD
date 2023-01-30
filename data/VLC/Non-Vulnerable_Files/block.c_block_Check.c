}
static void block_Check (block_t *block)
{
    while (block != NULL)
    {
        unsigned char *start = block->p_start;
        unsigned char *end = block->p_start + block->i_size;
        unsigned char *bufstart = block->p_buffer;
        unsigned char *bufend = block->p_buffer + block->i_buffer;
        assert (block->pf_release != BlockNoRelease);
        assert (start <= end);
        assert (bufstart <= bufend);
        assert (bufstart >= start);
        assert (bufend <= end);
        block = block->p_next;
    }
}
