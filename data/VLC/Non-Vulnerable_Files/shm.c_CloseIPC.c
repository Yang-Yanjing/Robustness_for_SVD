}
static void CloseIPC (demux_sys_t *sys)
{
    shmdt (sys->mem.addr);
}
