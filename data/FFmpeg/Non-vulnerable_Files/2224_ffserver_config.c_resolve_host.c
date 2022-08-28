static int resolve_host(struct in_addr *sin_addr, const char *hostname)
{
    if (!ff_inet_aton(hostname, sin_addr)) {
#if HAVE_GETADDRINFO
        struct addrinfo *ai, *cur;
        struct addrinfo hints = { 0 };
        hints.ai_family = AF_INET;
        if (getaddrinfo(hostname, NULL, &hints, &ai))
            return -1;
        


        for (cur = ai; cur; cur = cur->ai_next) {
            if (cur->ai_family == AF_INET) {
                *sin_addr = ((struct sockaddr_in *)cur->ai_addr)->sin_addr;
                freeaddrinfo(ai);
                return 0;
            }
        }
        freeaddrinfo(ai);
        return -1;
#else
        struct hostent *hp;
        hp = gethostbyname(hostname);
        if (!hp)
            return -1;
        memcpy(sin_addr, hp->h_addr_list[0], sizeof(struct in_addr));
#endif
    }
    return 0;
}
