}
int *net_Listen (vlc_object_t *p_this, const char *psz_host,
                 int i_port, int type, int protocol)
{
    struct addrinfo hints = {
        .ai_socktype = type,
        .ai_protocol = protocol,
        .ai_flags = AI_PASSIVE | AI_NUMERICSERV | AI_IDN,
    }, *res;
    msg_Dbg (p_this, "net: listening to %s port %d",
             (psz_host != NULL) ? psz_host : "*", i_port);
    int i_val = vlc_getaddrinfo (psz_host, i_port, &hints, &res);
    if (i_val)
    {
        msg_Err (p_this, "Cannot resolve %s port %d : %s",
                 (psz_host != NULL) ? psz_host : "", i_port,
                 gai_strerror (i_val));
        return NULL;
    }
    int *sockv = NULL;
    unsigned sockc = 0;
    for (struct addrinfo *ptr = res; ptr != NULL; ptr = ptr->ai_next)
    {
        int fd = net_Socket (p_this, ptr->ai_family, ptr->ai_socktype,
                             ptr->ai_protocol);
        if (fd == -1)
        {
            msg_Dbg (p_this, "socket error: %s", vlc_strerror_c(net_errno));
            continue;
        }
        /* Bind the socket */
#if defined (_WIN32)
        /*
         * Under Win32 and for multicasting, we bind to INADDR_ANY.
         * This is of course a severe bug, since the socket would logically
         * receive unicast traffic, and multicast traffic of groups subscribed
         * to via other sockets.
         */
        if (net_SockAddrIsMulticast (ptr->ai_addr, ptr->ai_addrlen)
         && (sizeof (struct sockaddr_storage) >= ptr->ai_addrlen))
        {
            // This works for IPv4 too - don't worry!
            struct sockaddr_in6 dumb =
            {
                .sin6_family = ptr->ai_addr->sa_family,
                .sin6_port =  ((struct sockaddr_in *)(ptr->ai_addr))->sin_port
            };
            bind (fd, (struct sockaddr *)&dumb, ptr->ai_addrlen);
        }
        else
#endif
        if (bind (fd, ptr->ai_addr, ptr->ai_addrlen))
        {
            net_Close (fd);
#if !defined(_WIN32)
            fd = rootwrap_bind (ptr->ai_family, ptr->ai_socktype,
                                ptr->ai_protocol,
                                ptr->ai_addr, ptr->ai_addrlen);
            if (fd != -1)
            {
                msg_Dbg (p_this, "got socket %d from rootwrap", fd);
            }
            else
#endif
            {
                msg_Err (p_this, "socket bind error: %s",
                         vlc_strerror_c(net_errno));
                continue;
            }
        }
        if (net_SockAddrIsMulticast (ptr->ai_addr, ptr->ai_addrlen))
        {
            if (net_Subscribe (p_this, fd, ptr->ai_addr, ptr->ai_addrlen))
            {
                net_Close (fd);
                continue;
            }
        }
        /* Listen */
        switch (ptr->ai_socktype)
        {
            case SOCK_STREAM:
            case SOCK_RDM:
            case SOCK_SEQPACKET:
#ifdef SOCK_DCCP
            case SOCK_DCCP:
#endif
                if (listen (fd, INT_MAX))
                {
                    msg_Err (p_this, "socket listen error: %s",
                             vlc_strerror_c(net_errno));
                    net_Close (fd);
                    continue;
                }
        }
        int *nsockv = (int *)realloc (sockv, (sockc + 2) * sizeof (int));
        if (nsockv != NULL)
        {
            nsockv[sockc++] = fd;
            sockv = nsockv;
        }
        else
            net_Close (fd);
    }
    freeaddrinfo (res);
    if (sockv != NULL)
        sockv[sockc] = -1;
    return sockv;
}
