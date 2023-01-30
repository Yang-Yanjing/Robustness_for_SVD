#endif
static void *Thread (void *data)
{
    stream_t *stream = data;
    stream_sys_t *p_sys = stream->p_sys;
#ifdef HAVE_VMSPLICE
    const ssize_t page_mask = sysconf (_SC_PAGE_SIZE) - 1;
#endif
    int fd = p_sys->write_fd;
    bool error = false;
    do
    {
        ssize_t len;
        int canc = vlc_savecancel ();
#ifdef HAVE_VMSPLICE
        unsigned char *buf = mmap (NULL, bufsize, PROT_READ|PROT_WRITE,
                                   MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (unlikely(buf == MAP_FAILED))
            break;
        vlc_cleanup_push (cleanup_mmap, buf);
#else
        unsigned char *buf = malloc (bufsize);
        if (unlikely(buf == NULL))
            break;
        vlc_cleanup_push (free, buf);
#endif
        vlc_mutex_lock (&p_sys->lock);
        while (p_sys->paused) /* practically always false, but... */
            vlc_cond_wait (&p_sys->wait, &p_sys->lock);
        len = stream_Read (stream->p_source, buf, bufsize);
        vlc_mutex_unlock (&p_sys->lock);
        vlc_restorecancel (canc);
        error = len <= 0;
        for (ssize_t i = 0, j; i < len; i += j)
        {
#ifdef HAVE_VMSPLICE
            if ((len - i) <= page_mask) /* incomplete last page */
                j = write (fd, buf + i, len - i);
            else
            {
                struct iovec iov = { buf + i, (len - i) & ~page_mask, };
                j = vmsplice (fd, &iov, 1, SPLICE_F_GIFT);
            }
            if (j == -1 && errno == ENOSYS) /* vmsplice() not supported */
#endif
            j = write (fd, buf + i, len - i);
            if (j <= 0)
            {
                if (j == 0)
                    errno = EPIPE;
                msg_Err (stream, "cannot write data: %s",
                         vlc_strerror_c(errno));
                error = true;
                break;
            }
        }
        vlc_cleanup_run (); /* free (buf) */
    }
    while (!error);
    msg_Dbg (stream, "compressed stream at EOF");
    /* Let child process know about EOF */
    p_sys->write_fd = -1;
    close (fd);
    return NULL;
}
