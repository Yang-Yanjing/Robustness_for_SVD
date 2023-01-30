                          const struct sockaddr *addr, size_t alen);
int net_Socket (vlc_object_t *p_this, int family, int socktype,
                int protocol)
{
    int fd = vlc_socket (family, socktype, protocol, true);
    if (fd == -1)
    {
        if (net_errno != EAFNOSUPPORT)
            msg_Err (p_this, "cannot create socket: %s",
                     vlc_strerror_c(net_errno));
        return -1;
    }
    setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof (int));
#ifdef IPV6_V6ONLY
    /*
     * Accepts only IPv6 connections on IPv6 sockets.
     * If possible, we should open two sockets, but it is not always possible.
     */
    if (family == AF_INET6)
        setsockopt (fd, IPPROTO_IPV6, IPV6_V6ONLY, &(int){ 1 }, sizeof (int));
#endif
#if defined (_WIN32)
# ifndef IPV6_PROTECTION_LEVEL
#  warning Please update your C library headers.
#  define IPV6_PROTECTION_LEVEL 23
#  define PROTECTION_LEVEL_UNRESTRICTED 10
# endif
    if (family == AF_INET6)
        setsockopt (fd, IPPROTO_IPV6, IPV6_PROTECTION_LEVEL,
                    &(int){ PROTECTION_LEVEL_UNRESTRICTED }, sizeof (int));
#endif
#ifdef DCCP_SOCKOPT_SERVICE
    if (socktype == SOL_DCCP)
    {
        char *dccps = var_InheritString (p_this, "dccp-service");
        if (dccps != NULL)
        {
            setsockopt (fd, SOL_DCCP, DCCP_SOCKOPT_SERVICE, dccps,
                        (strlen (dccps) + 3) & ~3);
            free (dccps);
        }
    }
#endif
    return fd;
}
