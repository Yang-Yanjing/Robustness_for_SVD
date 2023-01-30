static struct addrinfo *
makeaddrinfo (int af, int type, int proto,
              const struct sockaddr *addr, size_t addrlen,
              const char *canonname)
{
    struct addrinfo *res;
    res = (struct addrinfo *)malloc (sizeof (struct addrinfo));
    if (res != NULL)
    {
        res->ai_flags = 0;
        res->ai_family = af;
        res->ai_socktype = type;
        res->ai_protocol = proto;
        res->ai_addrlen = addrlen;
        res->ai_addr = malloc (addrlen);
        res->ai_canonname = NULL;
        res->ai_next = NULL;
        if (res->ai_addr != NULL)
        {
            memcpy (res->ai_addr, addr, addrlen);
            if (canonname != NULL)
            {
                res->ai_canonname = strdup (canonname);
                if (res->ai_canonname != NULL)
                    return res; /* success ! */
            }
            else
                return res;
        }
    }
    /* failsafe */
    freeaddrinfo (res);
    return NULL;
}
