 */
block_t *block_heap_Alloc (void *addr, size_t length)
{
    block_t *block = malloc (sizeof (*block));
    if (block == NULL)
    {
        free (addr);
        return NULL;
    }
    block_Init (block, addr, length);
    block->pf_release = block_heap_Release;
    return block;
}
