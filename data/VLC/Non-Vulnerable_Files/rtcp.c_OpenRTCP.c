};
rtcp_sender_t *OpenRTCP (vlc_object_t *obj, int rtp_fd, int proto,
                         bool mux)
{
    rtcp_sender_t *rtcp;
    uint8_t *ptr;
    int fd;
    char src[NI_MAXNUMERICHOST];
    int sport;
    if (net_GetSockAddress (rtp_fd, src, &sport))
        return NULL;
    if (mux)
    {
        /* RTP/RTCP mux: duplicate the socket */
#ifndef _WIN32
        fd = vlc_dup (rtp_fd);
#else
        WSAPROTOCOL_INFO info;
        WSADuplicateSocket (rtp_fd, GetCurrentProcessId (), &info);
        fd = WSASocket (info.iAddressFamily, info.iSocketType, info.iProtocol,
                        &info, 0, 0);
#endif
    }
    else
    {
        /* RTCP on a separate port */
        char dst[NI_MAXNUMERICHOST];
        int dport;
        if (net_GetPeerAddress (rtp_fd, dst, &dport))
            return NULL;
        sport++;
        dport++;
        fd = net_OpenDgram (obj, src, sport, dst, dport, proto);
        if (fd != -1)
        {
            /* Copy the multicast IPv4 TTL value (useless for IPv6) */
            int ttl;
            socklen_t len = sizeof (ttl);
            if (!getsockopt (rtp_fd, SOL_IP, IP_MULTICAST_TTL, &ttl, &len))
                setsockopt (fd, SOL_IP, IP_MULTICAST_TTL, &ttl, len);
            /* Ignore all incoming RTCP-RR packets */
            setsockopt (fd, SOL_SOCKET, SO_RCVBUF, &(int){ 0 }, sizeof (int));
        }
    }
    if (fd == -1)
        return NULL;
    rtcp = malloc (sizeof (*rtcp));
    if (rtcp == NULL)
    {
        net_Close (fd);
        return NULL;
    }
    rtcp->handle = fd;
    rtcp->bytes = rtcp->packets = rtcp->counter = 0;
    ptr = (uint8_t *)strchr (src, '%');
    if (ptr != NULL)
        *ptr = '\0'; /* remove scope ID frop IPv6 addresses */
    ptr = rtcp->payload;
    /* Sender report */
    ptr[0] = 2 << 6; /* V = 2, P = RC = 0 */
    ptr[1] = 200; /* payload type: Sender Report */
    SetWBE (ptr + 2, 6); /* length = 6 (7 double words) */
    memset (ptr + 4, 0, 4); /* SSRC unknown yet */
    SetQWBE (ptr + 8, NTPtime64 ());
    memset (ptr + 16, 0, 12); /* timestamp and counters */
    ptr += 28;
    /* Source description */
    uint8_t *sdes = ptr;
    ptr[0] = (2 << 6) | 1; /* V = 2, P = 0, SC = 1 */
    ptr[1] = 202; /* payload type: Source Description */
    uint8_t *lenptr = ptr + 2;
    memset (ptr + 4, 0, 4); /* SSRC unknown yet */
    ptr += 8;
    ptr[0] = 1; /* CNAME - mandatory */
    assert (NI_MAXNUMERICHOST <= 256);
    ptr[1] = strlen (src);
    memcpy (ptr + 2, src, ptr[1]);
    ptr += ptr[1] + 2;
    static const char tool[] = PACKAGE_STRING;
    ptr[0] = 6; /* TOOL */
    ptr[1] = (sizeof (tool) > 256) ? 255 : (sizeof (tool) - 1);
    memcpy (ptr + 2, tool, ptr[1]);
    ptr += ptr[1] + 2;
    while ((ptr - sdes) & 3) /* 32-bits padding */
        *ptr++ = 0;
    SetWBE (lenptr, (ptr - sdes - 1) >> 2);
    rtcp->length = ptr - rtcp->payload;
    return rtcp;
}
