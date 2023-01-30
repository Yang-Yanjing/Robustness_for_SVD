 */
int rootwrap_bind (int family, int socktype, int protocol,
                   const struct sockaddr *addr, size_t alen)
{
    /* can't use libvlc */
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    struct sockaddr_storage ss;
    int fd, sock = -1;
    const char *sockenv = getenv ("VLC_ROOTWRAP_SOCK");
    if (sockenv != NULL)
        sock = atoi (sockenv);
    if (sock == -1)
    {
        errno = EACCES;
        return -1;
    }
    switch (family)
    {
        case AF_INET:
            if (alen < sizeof (struct sockaddr_in))
            {
                errno = EINVAL;
                return -1;
            }
            break;
#ifdef AF_INET6
        case AF_INET6:
            if (alen < sizeof (struct sockaddr_in6))
            {
                errno = EINVAL;
                return -1;
            }
            break;
#endif
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
    if (family != addr->sa_family)
    {
        errno = EAFNOSUPPORT;
        return -1;
    }
    /* Only TCP is implemented at the moment */
    if ((socktype != SOCK_STREAM)
     || (protocol && (protocol != IPPROTO_TCP)))
    {
        errno = EACCES;
        return -1;
    }
    memset (&ss, 0, sizeof (ss));
    memcpy (&ss, addr, (alen > sizeof (ss)) ? sizeof (ss) : alen);
    pthread_mutex_lock (&mutex);
    if (send (sock, &ss, sizeof (ss), 0) != sizeof (ss))
    {
        pthread_mutex_unlock (&mutex);
        return -1;
    }
    fd = recv_fd (sock);
    pthread_mutex_unlock (&mutex);
    return fd;
}
#else
int rootwrap_bind (int family, int socktype, int protocol,
                   const struct sockaddr *addr, size_t alen)
{
    (void)family;
    (void)socktype;
    (void)protocol;
    (void)addr;
    (void)alen;
    errno = EACCES;
    return -1;
}
