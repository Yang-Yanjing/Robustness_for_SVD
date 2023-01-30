#ifdef HAVE_VMSPLICE
static void cleanup_mmap (void *addr)
{
    munmap (addr, bufsize);
}
