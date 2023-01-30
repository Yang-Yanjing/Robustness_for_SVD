static int read_random(uint32_t *dst, const char *file)
{
#if HAVE_UNISTD_H
    int fd = avpriv_open(file, O_RDONLY);
    int err = -1;
    if (fd == -1)
        return -1;
    err = read(fd, dst, sizeof(*dst));
    close(fd);
    return err;
#else
    return -1;
#endif
}
