ssize_t
net_Read (vlc_object_t *restrict p_this, int fd, const v_socket_t *vs,
          void *restrict p_buf, size_t i_buflen, bool waitall)
{
    struct pollfd ufd[2];
    ufd[0].fd = fd;
    ufd[0].events = POLLIN;
    ufd[1].fd = vlc_object_waitpipe (p_this);
    ufd[1].events = POLLIN;
    size_t i_total = 0;
#if VLC_WINSTORE_APP
    /* With winrtsock winsocks emulation library, the first call to read()
     * before poll() starts an asynchronous transfer and returns 0.
     * Always call poll() first.
     *
     * However if we have a virtual socket handler, try to read() first.
     * See bug #8972 for details.
     */
    if (vs == NULL)
        goto do_poll;
#endif
    do
    {
        ssize_t n;
        if (vs != NULL)
        {
            int canc = vlc_savecancel ();
            n = vs->pf_recv (vs->p_sys, p_buf, i_buflen);
            vlc_restorecancel (canc);
        }
        else
        {
#ifdef _WIN32
            n = recv (fd, p_buf, i_buflen, 0);
#else
            n = read (fd, p_buf, i_buflen);
#endif
        }
        if (n < 0)
        {
            switch (net_errno)
            {
                case EAGAIN: /* no data */
#if (EAGAIN != EWOULDBLOCK)
                case EWOULDBLOCK:
#endif
                    break;
#ifndef _WIN32
                case EINTR:  /* asynchronous signal */
                    continue;
#else
                case WSAEMSGSIZE: /* datagram too big */
                    n = i_buflen;
                    break;
#endif
                default:
                    goto error;
            }
        }
        else
        if (n > 0)
        {
            i_total += n;
            p_buf = (char *)p_buf + n;
            i_buflen -= n;
            if (!waitall || i_buflen == 0)
                break;
        }
        else /* n == 0 */
            break;/* end of stream or empty packet */
        if (ufd[1].fd == -1)
        {
            errno = EINTR;
            return -1;
        }
#if VLC_WINSTORE_APP
do_poll:
#endif
        /* Wait for more data */
        if (poll (ufd, sizeof (ufd) / sizeof (ufd[0]), -1) < 0)
        {
            if (errno == EINTR)
                continue;
            goto error;
        }
        if (ufd[1].revents)
        {
            msg_Dbg (p_this, "socket %d polling interrupted", fd);
            errno = EINTR;
            return -1;
        }
        assert (ufd[0].revents);
    }
    while (i_buflen > 0);
    return i_total;
error:
    msg_Err (p_this, "read error: %s", vlc_strerror_c(errno));
    return -1;
}
