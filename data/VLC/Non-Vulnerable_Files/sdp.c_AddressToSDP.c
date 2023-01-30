static
char *AddressToSDP (const struct sockaddr *addr, socklen_t addrlen, char *buf)
{
    if (addrlen < offsetof (struct sockaddr, sa_family)
                 + sizeof (addr->sa_family))
        return NULL;
    strcpy (buf, "IN IP* ");
    if (vlc_getnameinfo (addr, addrlen, buf + 7, MAXSDPADDRESS - 7, NULL,
                         NI_NUMERICHOST))
        return NULL;
    switch (addr->sa_family)
    {
        case AF_INET:
        {
            if (net_SockAddrIsMulticast (addr, addrlen))
                strcat (buf, "/255"); // obsolete in RFC4566, dummy value
            buf[5] = '4';
            break;
        }
#ifdef AF_INET6
        case AF_INET6:
        {
            char *ptr = strchr (buf, '%');
            if (ptr != NULL)
                *ptr = '\0'; // remove scope ID
            buf[5] = '6';
            break;
        }
#endif
        default:
            return NULL;
    }
    return buf;
}
