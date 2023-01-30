 */
block_t *block_mmap_Alloc (void *addr, size_t length)
{
    if (addr == MAP_FAILED)
        return NULL;
    block_t *block = malloc (sizeof (*block));
    if (block == NULL)
    {
        munmap (addr, length);
        return NULL;
    }
    block_Init (block, addr, length);
    block->pf_release = block_mmap_Release;
    return block;
}
#else
block_t *block_mmap_Alloc (void *addr, size_t length)
{
    (void)addr; (void)length; return NULL;
}
