}
static gme_err_t ReaderBlock (void *data, void *buf, int length)
{
    block_t *block = data;
    int max = __MIN (length, (int)block->i_buffer);
    memcpy (buf, block->p_buffer, max);
    block->i_buffer -= max;
    block->p_buffer += max;
    if (max != length)
        return "short read";
    return NULL;
}
