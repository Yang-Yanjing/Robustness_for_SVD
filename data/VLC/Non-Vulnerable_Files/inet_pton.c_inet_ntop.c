}
const char *inet_ntop (int af, const void *src, char *dst, int len)
{
    const unsigned char *b = src;
    switch (af)
    {
        case AF_INET:
            if (snprintf (dst, len, "%hhu.%hhu.%hhu.%hhu",
                          b[0], b[1], b[2], b[3]) >= len)
            {
                errno = ENOSPC;
                return NULL;
            }
            return dst;
    }
    errno = EAFNOSUPPORT;
    return NULL;
}
