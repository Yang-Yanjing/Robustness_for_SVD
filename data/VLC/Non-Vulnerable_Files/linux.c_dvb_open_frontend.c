/** Finds a frontend of the correct type */
static int dvb_open_frontend (dvb_device_t *d)
{
    if (d->frontend != -1)
        return 0;
    int fd = dvb_open_node (d, "frontend", O_RDWR);
    if (fd == -1)
    {
        msg_Err (d->obj, "cannot access frontend: %s", vlc_strerror_c(errno));
        return -1;
    }
    d->frontend = fd;
    return 0;
}
