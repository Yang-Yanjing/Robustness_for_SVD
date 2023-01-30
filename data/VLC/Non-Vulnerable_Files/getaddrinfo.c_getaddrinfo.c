int
getaddrinfo (const char *node, const char *service,
             const struct addrinfo *hints, struct addrinfo **res)
{
    struct addrinfo *info;
    u_long ip;
    u_short port;
    int protocol = 0, flags = 0;
    const char *name = NULL;
    if (hints != NULL)
    {
        flags = hints->ai_flags;
        if (flags & ~_AI_MASK)
            return EAI_BADFLAGS;
        /* only accept AF_INET and AF_UNSPEC */
        if (hints->ai_family && (hints->ai_family != AF_INET))
            return EAI_FAMILY;
        /* protocol sanity check */
        switch (hints->ai_socktype)
        {
            case SOCK_STREAM:
                protocol = IPPROTO_TCP;
                break;
            case SOCK_DGRAM:
                protocol = IPPROTO_UDP;
                break;
#ifdef SOCK_RAW
            case SOCK_RAW:
#endif
            case 0:
                break;
            default:
                return EAI_SOCKTYPE;
        }
        if (hints->ai_protocol && protocol
         && (protocol != hints->ai_protocol))
            return EAI_SERVICE;
    }
    *res = NULL;
    /* default values */
    if (node == NULL)
    {
        if (flags & AI_PASSIVE)
            ip = htonl (INADDR_ANY);
        else
            ip = htonl (INADDR_LOOPBACK);
    }
    else
    if ((ip = inet_addr (node)) == INADDR_NONE)
    {
        struct hostent *entry = NULL;
        /* hostname resolution */
        if (!(flags & AI_NUMERICHOST))
            entry = gethostbyname (node);
        if (entry == NULL)
            return gai_error_from_herrno ();
        if ((entry->h_length != 4) || (entry->h_addrtype != AF_INET))
            return EAI_FAMILY;
        ip = *((u_long *) entry->h_addr);
        if (flags & AI_CANONNAME)
            name = entry->h_name;
    }
    if ((flags & AI_CANONNAME) && (name == NULL))
        name = node;
    /* service resolution */
    if (service == NULL)
        port = 0;
    else
    {
        unsigned long d;
        char *end;
        d = strtoul (service, &end, 0);
        if (end[0] || (d > 65535u))
            return EAI_SERVICE;
        port = htons ((u_short)d);
    }
    /* building results... */
    if ((!protocol) || (protocol == IPPROTO_UDP))
    {
        info = makeipv4info (SOCK_DGRAM, IPPROTO_UDP, ip, port, name);
        if (info == NULL)
        {
            errno = ENOMEM;
            return EAI_SYSTEM;
        }
        if (flags & AI_PASSIVE)
            info->ai_flags |= AI_PASSIVE;
        *res = info;
    }
    if ((!protocol) || (protocol == IPPROTO_TCP))
    {
        info = makeipv4info (SOCK_STREAM, IPPROTO_TCP, ip, port, name);
        if (info == NULL)
        {
            errno = ENOMEM;
            return EAI_SYSTEM;
        }
        info->ai_next = *res;
        if (flags & AI_PASSIVE)
            info->ai_flags |= AI_PASSIVE;
        *res = info;
    }
    return 0;
}
