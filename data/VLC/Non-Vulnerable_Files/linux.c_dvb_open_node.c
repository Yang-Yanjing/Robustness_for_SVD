/** Opens the DVB device node of the specified type */
static int dvb_open_node (dvb_device_t *d, const char *type, int flags)
{
    int fd;
    char path[strlen (type) + 4];
    snprintf (path, sizeof (path), "%s%u", type, d->device);
    fd = vlc_openat (d->dir, path, flags);
    if (fd != -1)
        fcntl (fd, F_SETFL, fcntl (fd, F_GETFL) | O_NONBLOCK);
    return fd;
}
