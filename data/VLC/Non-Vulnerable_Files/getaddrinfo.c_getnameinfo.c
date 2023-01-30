int
getnameinfo (const struct sockaddr *sa, socklen_t salen,
                 char *host, int hostlen, char *serv, int servlen, int flags)
{
    if (((size_t)salen < sizeof (struct sockaddr_in))
     || (sa->sa_family != AF_INET))
        return EAI_FAMILY;
    else if (flags & (~_NI_MASK))
        return EAI_BADFLAGS;
    else
    {
        const struct sockaddr_in *addr;
        addr = (const struct sockaddr_in *)sa;
        if (host != NULL)
        {
            /* host name resolution */
            if (!(flags & NI_NUMERICHOST))
            {
                if (flags & NI_NAMEREQD)
                    return EAI_NONAME;
            }
            /* inet_ntoa() is not thread-safe, do not use it */
            uint32_t ipv4 = ntohl (addr->sin_addr.s_addr);
            if (snprintf (host, hostlen, "%u.%u.%u.%u", ipv4 >> 24,
                          (ipv4 >> 16) & 0xff, (ipv4 >> 8) & 0xff,
                          ipv4 & 0xff) >= (int)hostlen)
                return EAI_OVERFLOW;
        }
        if (serv != NULL)
        {
            if (snprintf (serv, servlen, "%u",
                          (unsigned int)ntohs (addr->sin_port)) >= (int)servlen)
                return EAI_OVERFLOW;
        }
    }
    return 0;
}
