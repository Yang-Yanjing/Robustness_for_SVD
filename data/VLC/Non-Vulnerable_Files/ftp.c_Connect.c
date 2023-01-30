}
static int Connect( vlc_object_t *p_access, access_sys_t *p_sys )
{
    if( Login( p_access, p_sys ) < 0 )
        return -1;
    /* Extended passive mode */
    if( ftp_SendCommand( p_access, p_sys, "EPSV ALL" ) < 0 )
    {
        msg_Err( p_access, "cannot request extended passive mode" );
        goto error;
    }
    if( ftp_RecvCommand( p_access, p_sys, NULL, NULL ) == 2 )
    {
        if( net_GetPeerAddress( p_sys->cmd.fd, p_sys->sz_epsv_ip, NULL ) )
            goto error;
    }
    else
    {
        /* If ESPV ALL fails, we fallback to PASV.
         * We have to restart the connection in case there is a NAT that
         * understands EPSV ALL in the way, and hence won't allow PASV on
         * the initial connection.
         */
        msg_Info( p_access, "FTP Extended passive mode disabled" );
        clearCmdTLS( p_sys );
        net_Close( p_sys->cmd.fd );
        if( Login( p_access, p_sys ) )
            goto error;
    }
    if( (p_sys->features.b_unicode ? IsUTF8 : IsASCII)(p_sys->url.psz_path) == NULL )
    {
        msg_Err( p_access, "unsupported path: \"%s\"", p_sys->url.psz_path );
        goto error;
    }
    /* check binary mode support */
    if( ftp_SendCommand( p_access, p_sys, "TYPE I" ) < 0 ||
        ftp_RecvCommand( p_access, p_sys, NULL, NULL ) != 2 )
    {
        msg_Err( p_access, "cannot set binary transfer mode" );
        goto error;
    }
    return 0;
error:
    clearCmdTLS( p_sys );
    net_Close( p_sys->cmd.fd );
    return -1;
}
