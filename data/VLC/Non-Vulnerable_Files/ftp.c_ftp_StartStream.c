}
static int ftp_StartStream( vlc_object_t *p_access, access_sys_t *p_sys,
                            uint64_t i_start )
{
    char psz_ipv4[16], *psz_ip = p_sys->sz_epsv_ip;
    int  i_answer;
    char *psz_arg, *psz_parser;
    int  i_port;
    assert( p_sys->data.fd == -1 );
    if( ( ftp_SendCommand( p_access, p_sys, *psz_ip ? "EPSV" : "PASV" ) < 0 )
     || ( ftp_RecvCommand( p_access, p_sys, &i_answer, &psz_arg ) != 2 ) )
    {
        msg_Err( p_access, "cannot set passive mode" );
        return VLC_EGENERIC;
    }
    psz_parser = strchr( psz_arg, '(' );
    if( psz_parser == NULL )
    {
        free( psz_arg );
        msg_Err( p_access, "cannot parse passive mode response" );
        return VLC_EGENERIC;
    }
    if( *psz_ip )
    {
        if( sscanf( psz_parser, "(%*3c%u", &i_port ) < 1 )
        {
            free( psz_arg );
            msg_Err( p_access, "cannot parse passive mode response" );
            return VLC_EGENERIC;
        }
    }
    else
    {
        unsigned a1, a2, a3, a4, p1, p2;
        if( ( sscanf( psz_parser, "(%u,%u,%u,%u,%u,%u", &a1, &a2, &a3, &a4,
                      &p1, &p2 ) < 6 ) || ( a1 > 255 ) || ( a2 > 255 )
         || ( a3 > 255 ) || ( a4 > 255 ) || ( p1 > 255 ) || ( p2 > 255 ) )
        {
            free( psz_arg );
            msg_Err( p_access, "cannot parse passive mode response" );
            return VLC_EGENERIC;
        }
        sprintf( psz_ipv4, "%u.%u.%u.%u", a1, a2, a3, a4 );
        psz_ip = psz_ipv4;
        i_port = (p1 << 8) | p2;
    }
    free( psz_arg );
    msg_Dbg( p_access, "ip:%s port:%d", psz_ip, i_port );
    if( ftp_SendCommand( p_access, p_sys, "TYPE I" ) < 0 ||
        ftp_RecvCommand( p_access, p_sys, &i_answer, NULL ) != 2 )
    {
        msg_Err( p_access, "cannot set binary transfer mode" );
        return VLC_EGENERIC;
    }
    if( i_start > 0 )
    {
        if( ftp_SendCommand( p_access, p_sys, "REST %"PRIu64, i_start ) < 0 ||
            ftp_RecvCommand( p_access, p_sys, &i_answer, NULL ) > 3 )
        {
            msg_Err( p_access, "cannot set restart offset" );
            return VLC_EGENERIC;
        }
    }
    msg_Dbg( p_access, "waiting for data connection..." );
    p_sys->data.fd = net_ConnectTCP( p_access, psz_ip, i_port );
    if( p_sys->data.fd < 0 )
    {
        msg_Err( p_access, "failed to connect with server" );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_access, "connection with \"%s:%d\" successful",
             psz_ip, i_port );
    if( p_sys->directory )
    {
        if( ftp_SendCommand( p_access, p_sys, "NLST" ) < 0 ||
            ftp_RecvCommand( p_access, p_sys, NULL, &psz_arg ) > 2 )
        {
            msg_Err( p_access, "cannot list directory contents" );
        return VLC_EGENERIC;
        }
    }
    else
    {
        /* "1xx" message */
        assert( p_sys->url.psz_path );
        if( ftp_SendCommand( p_access, p_sys, "%s %s",
                             p_sys->out ? "STOR" : "RETR",
                             p_sys->url.psz_path ) < 0
         || ftp_RecvCommand( p_access, p_sys, &i_answer, NULL ) > 2 )
        {
            msg_Err( p_access, "cannot retrieve file" );
            return VLC_EGENERIC;
        }
    }
    if( p_sys->tlsmode != NONE )
    {
        /* FIXME: Do Reuse TLS Session */
        /* TLS/SSL handshake */
        p_sys->data.p_tls = vlc_tls_ClientSessionCreate( p_sys->p_creds,
                            p_sys->data.fd, p_sys->url.psz_host,
                            ( p_sys->tlsmode == EXPLICIT ) ? "ftpes-data"
                                                           : "ftps-data" );
        if( p_sys->data.p_tls == NULL )
        {
            msg_Err( p_access, "cannot establish FTP/TLS session for data" \
                             ": server not allowing new session ?" );
            return VLC_EGENERIC;
        }
        p_sys->data.p_vs = &p_sys->data.p_tls->sock;
    }
    else
        shutdown( p_sys->data.fd, p_sys->out ? SHUT_RD : SHUT_WR );
    return VLC_SUCCESS;
}
