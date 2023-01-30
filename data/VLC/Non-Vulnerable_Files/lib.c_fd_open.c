int (*v4l2_munmap) (void *, size_t);
static int fd_open (int fd, int flags)
{
    (void) flags;
    return fd;
}
