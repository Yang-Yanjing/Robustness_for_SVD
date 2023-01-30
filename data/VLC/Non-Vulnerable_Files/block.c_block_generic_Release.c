}
static void block_generic_Release (block_t *block)
{
    /* That is always true for blocks allocated with block_Alloc(). */
    assert (block->p_start == (unsigned char *)(block + 1));
    block_Invalidate (block);
    free (block);
}
