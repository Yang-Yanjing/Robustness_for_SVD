# include <sys/mman.h>
static void block_mmap_Release (block_t *block)
{
    block_Invalidate (block);
    munmap (block->p_start, block->i_size);
    free (block);
}
