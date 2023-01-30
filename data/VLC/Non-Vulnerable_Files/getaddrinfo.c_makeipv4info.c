static struct addrinfo *
makeipv4info (int type, int proto, u_long ip, u_short port, const char *name)
{
    struct sockaddr_in addr;
    memset (&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
# ifdef HAVE_SA_LEN
    addr.sin_len = sizeof (addr);
# endif
    addr.sin_port = port;
    addr.sin_addr.s_addr = ip;
    return makeaddrinfo (AF_INET, type, proto,
                         (struct sockaddr*)&addr, sizeof (addr), name);
}
