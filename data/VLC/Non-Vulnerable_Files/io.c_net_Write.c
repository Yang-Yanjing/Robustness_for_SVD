 */
ssize_t net_Write( vlc_object_t *p_this, int fd, const v_socket_t *p_vs,
                   const void *restrict p_data, size_t i_data )
{
    size_t i_total = 0;
    struct pollfd ufd[2] = {
        { .fd = fd,                           .events = POLLOUT },
        { .fd = vlc_object_waitpipe (p_this), .events = POLLIN  },
    };
    if (unlikely(ufd[1].fd == -1))
    {
        vlc_testcancel ();
        return -1;
    }
    while( i_data > 0 )
    {
        ssize_t val;
        ufd[0].revents = ufd[1].revents = 0;
        if (poll (ufd, sizeof (ufd) / sizeof (ufd[0]), -1) == -1)
        {
            if (errno == EINTR)
                continue;
            msg_Err (p_this, "Polling error: %s", vlc_strerror_c(errno));
            return -1;
        }
        if (i_total > 0)
        {   /* If POLLHUP resp. POLLERR|POLLNVAL occurs while we have already
             * read some data, it is important that we first return the number
             * of bytes read, and then return 0 resp. -1 on the NEXT call. */
            if (ufd[0].revents & (POLLHUP|POLLERR|POLLNVAL))
                break;
            if (ufd[1].revents) /* VLC object signaled */
                break;
        }
        else
        {
            if (ufd[1].revents)
            {
                errno = EINTR;
                goto error;
            }
        }
        if (p_vs != NULL)
            val = p_vs->pf_send (p_vs->p_sys, p_data, i_data);
        else
#ifdef _WIN32
            val = send (fd, p_data, i_data, 0);
#else
            val = write (fd, p_data, i_data);
#endif
        if (val == -1)
        {
            if (errno == EINTR)
                continue;
            msg_Err (p_this, "Write error: %s", vlc_strerror_c(errno));
            break;
        }
        p_data = (const char *)p_data + val;
        i_data -= val;
        i_total += val;
    }
    if (unlikely(i_data == 0))
        vlc_testcancel (); /* corner case */
    if ((i_total > 0) || (i_data == 0))
        return i_total;
error:
    return -1;
}
