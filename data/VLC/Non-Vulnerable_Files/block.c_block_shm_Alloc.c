 */
block_t *block_shm_Alloc (void *addr, size_t length)
{
    block_shm_t *block = malloc (sizeof (*block));
    if (unlikely(block == NULL))
    {
        shmdt (addr);
        return NULL;
    }
    block_Init (&block->self, (uint8_t *)addr, length);
    block->self.pf_release = block_shm_Release;
    block->base_addr = addr;
    return &block->self;
}
#else
block_t *block_shm_Alloc (void *addr, size_t length)
{
    (void) addr; (void) length;
    abort ();
}
