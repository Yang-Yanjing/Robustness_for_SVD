 */
char *net_Gets(vlc_object_t *obj, int fd, const v_socket_t *vs)
{
    char *buf = NULL;
    size_t bufsize = 0, buflen = 0;
    for (;;)
    {
        if (buflen == bufsize)
        {
            if (unlikely(bufsize >= (1 << 16)))
                goto error; /* put sane buffer size limit */
            char *newbuf = realloc(buf, bufsize + 1024);
            if (unlikely(newbuf == NULL))
                goto error;
            buf = newbuf;
            bufsize += 1024;
        }
        ssize_t val = net_Read(obj, fd, vs, buf + buflen, 1, false);
        if (val < 1)
            goto error;
        if (buf[buflen] == '\n')
            break;
        buflen++;
    }
    buf[buflen] = '\0';
    if (buflen > 0 && buf[buflen - 1] == '\r')
        buf[buflen - 1] = '\0';
    return buf;
error:
    free(buf);
    return NULL;
}
