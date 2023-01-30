 */
int SAP_Add (sap_handler_t *p_sap, session_descriptor_t *p_session)
{
    int i;
    char psz_addr[NI_MAXNUMERICHOST];
    sap_session_t *p_sap_session;
    mtime_t i_hash;
    union
    {
        struct sockaddr     a;
        struct sockaddr_in  in;
        struct sockaddr_in6 in6;
    } addr;
    socklen_t addrlen;
    addrlen = p_session->addrlen;
    if ((addrlen == 0) || (addrlen > sizeof (addr)))
    {
        msg_Err( p_sap, "No/invalid address specified for SAP announce" );
        return VLC_EGENERIC;
    }
    /* Determine SAP multicast address automatically */
    memcpy (&addr, &p_session->addr, addrlen);
    switch (addr.a.sa_family)
    {
#if defined (HAVE_INET_PTON) || defined (_WIN32)
        case AF_INET6:
        {
            /* See RFC3513 for list of valid IPv6 scopes */
            struct in6_addr *a6 = &addr.in6.sin6_addr;
            memcpy( a6->s6_addr + 2, "\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x02\x7f\xfe", 14 );
            if( IN6_IS_ADDR_MULTICAST( a6 ) )
                /* force flags to zero, preserve scope */
                a6->s6_addr[1] &= 0xf;
            else
                /* Unicast IPv6 - assume global scope */
                memcpy( a6->s6_addr, "\xff\x0e", 2 );
            break;
        }
#endif
        case AF_INET:
        {
            /* See RFC2365 for IPv4 scopes */
            uint32_t ipv4 = addr.in.sin_addr.s_addr;
            /* 224.0.0.0/24 => 224.0.0.255 */
            if ((ipv4 & htonl (0xffffff00)) == htonl (0xe0000000))
                ipv4 =  htonl (0xe00000ff);
            else
            /* 239.255.0.0/16 => 239.255.255.255 */
            if ((ipv4 & htonl (0xffff0000)) == htonl (0xefff0000))
                ipv4 =  htonl (0xefffffff);
            else
            /* 239.192.0.0/14 => 239.195.255.255 */
            if ((ipv4 & htonl (0xfffc0000)) == htonl (0xefc00000))
                ipv4 =  htonl (0xefc3ffff);
            else
            if ((ipv4 & htonl (0xff000000)) == htonl (0xef000000))
                ipv4 = 0;
            else
            /* other addresses => 224.2.127.254 */
                ipv4 = htonl (0xe0027ffe);
            if( ipv4 == 0 )
            {
                msg_Err( p_sap, "Out-of-scope multicast address "
                         "not supported by SAP" );
                return VLC_EGENERIC;
            }
            addr.in.sin_addr.s_addr = ipv4;
            break;
        }
        default:
            msg_Err( p_sap, "Address family %d not supported by SAP",
                     addr.a.sa_family );
            return VLC_EGENERIC;
    }
    i = vlc_getnameinfo( &addr.a, addrlen,
                         psz_addr, sizeof( psz_addr ), NULL, NI_NUMERICHOST );
    if( i )
    {
        msg_Err( p_sap, "%s", gai_strerror( i ) );
        return VLC_EGENERIC;
    }
    /* Find/create SAP address thread */
    msg_Dbg( p_sap, "using SAP address: %s", psz_addr);
    vlc_mutex_lock (&p_sap->lock);
    sap_address_t *sap_addr;
    for (sap_addr = p_sap->first; sap_addr; sap_addr = sap_addr->next)
        if (!strcmp (psz_addr, sap_addr->group))
            break;
    if (sap_addr == NULL)
    {
        sap_addr = AddressCreate (VLC_OBJECT(p_sap), psz_addr);
        if (sap_addr == NULL)
        {
            vlc_mutex_unlock (&p_sap->lock);
            return VLC_EGENERIC;
        }
        sap_addr->next = p_sap->first;
        p_sap->first = sap_addr;
    }
    /* Switch locks.
     * NEVER take the global SAP lock when holding a SAP thread lock! */
    vlc_mutex_lock (&sap_addr->lock);
    vlc_mutex_unlock (&p_sap->lock);
    memcpy (&p_session->orig, &sap_addr->orig, sap_addr->origlen);
    p_session->origlen = sap_addr->origlen;
    size_t headsize = 20, length;
    switch (p_session->orig.ss_family)
    {
#ifdef AF_INET6
        case AF_INET6:
            headsize += 16;
            break;
#endif
        case AF_INET:
            headsize += 4;
            break;
        default:
            assert (0);
    }
    /* XXX: Check for dupes */
    length = headsize + strlen (p_session->psz_sdp);
    p_sap_session = malloc (sizeof (*p_sap_session) + length + 1);
    if (p_sap_session == NULL)
    {
        vlc_mutex_unlock (&sap_addr->lock);
        return VLC_EGENERIC; /* NOTE: we should destroy the thread if left unused */
    }
    p_sap_session->next = sap_addr->first;
    sap_addr->first = p_sap_session;
    p_sap_session->p_sd = p_session;
    p_sap_session->length = length;
    /* Build the SAP Headers */
    uint8_t *psz_head = p_sap_session->data;
    /* SAPv1, not encrypted, not compressed */
    psz_head[0] = 0x20;
    psz_head[1] = 0x00; /* No authentication length */
    i_hash = mdate();
    psz_head[2] = i_hash >> 8; /* Msg id hash */
    psz_head[3] = i_hash;      /* Msg id hash 2 */
    headsize = 4;
    switch (p_session->orig.ss_family)
    {
#ifdef AF_INET6
        case AF_INET6:
        {
            struct in6_addr *a6 =
                &((struct sockaddr_in6 *)&p_session->orig)->sin6_addr;
            memcpy (psz_head + headsize, a6, 16);
            psz_head[0] |= 0x10; /* IPv6 flag */
            headsize += 16;
            break;
        }
#endif
        case AF_INET:
        {
            uint32_t ipv4 =
                (((struct sockaddr_in *)&p_session->orig)->sin_addr.s_addr);
            memcpy (psz_head + headsize, &ipv4, 4);
            headsize += 4;
            break;
        }
    }
    memcpy (psz_head + headsize, "application/sdp", 16);
    headsize += 16;
    /* Build the final message */
    strcpy( (char *)psz_head + headsize, p_session->psz_sdp);
    sap_addr->session_count++;
    vlc_cond_signal (&sap_addr->wait);
    vlc_mutex_unlock (&sap_addr->lock);
    return VLC_SUCCESS;
}
