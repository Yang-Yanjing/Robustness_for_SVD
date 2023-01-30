}
int vlc_dup (int oldfd)
{
    int fd = dup (oldfd);
    if (fd != -1)
        setmode (fd, O_BINARY);
    return fd;
}
