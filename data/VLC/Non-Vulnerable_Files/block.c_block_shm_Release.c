} block_shm_t;
static void block_shm_Release (block_t *block)
{
    block_shm_t *p_sys = (block_shm_t *)block;
    shmdt (p_sys->base_addr);
    free (p_sys);
}
