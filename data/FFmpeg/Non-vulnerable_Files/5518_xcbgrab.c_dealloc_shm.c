static void dealloc_shm(void *unused, uint8_t *data)
{
    shmdt(data);
}
