 */
ssize_t dvb_read (dvb_device_t *d, void *buf, size_t len)
{
    struct pollfd ufd[2];
    int n;
#ifdef HAVE_DVBPSI
    if (d->cam != NULL)
        en50221_Poll (d->cam);
#endif
    ufd[0].fd = d->demux;
    ufd[0].events = POLLIN;
    if (d->frontend != -1)
    {
        ufd[1].fd = d->frontend;
        ufd[1].events = POLLIN;
        n = 2;
    }
    else
        n = 1;
    if (poll (ufd, n, 500 /* FIXME */) < 0)
        return -1;
    if (d->frontend != -1 && ufd[1].revents)
    {
        struct dvb_frontend_event ev;
        if (ioctl (d->frontend, FE_GET_EVENT, &ev) < 0)
        {
            if (errno == EOVERFLOW)
            {
                msg_Err (d->obj, "cannot dequeue events fast enough!");
                return -1;
            }
            msg_Err (d->obj, "cannot dequeue frontend event: %s",
                     vlc_strerror_c(errno));
            return 0;
        }
        msg_Dbg (d->obj, "frontend status: 0x%02X", (unsigned)ev.status);
    }
    if (ufd[0].revents)
    {
        ssize_t val = read (d->demux, buf, len);
        if (val == -1 && (errno != EAGAIN && errno != EINTR))
        {
            if (errno == EOVERFLOW)
            {
                msg_Err (d->obj, "cannot demux data fast enough!");
                return -1;
            }
            msg_Err (d->obj, "cannot demux: %s", vlc_strerror_c(errno));
            return 0;
        }
        return val;
    }
    return -1;
}
