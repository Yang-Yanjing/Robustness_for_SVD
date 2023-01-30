#endif
int inet_pton (int af, const char *src, void *dst)
{
    unsigned char *b = dst;
    switch (af)
    {
        case AF_INET:
            return sscanf (src, "%hhu.%hhu.%hhu.%hhu",
                           b + 0, b + 1, b + 2, b + 3) == 4;
    }
    errno = EAFNOSUPPORT;
    return -1;
}
