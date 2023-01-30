}
static int Login( vlc_object_t *p_access, access_sys_t *p_sys )
{
    int i_answer;
    /* *** Open a TCP connection with server *** */
    int fd = p_sys->cmd.fd = net_ConnectTCP( p_access, p_sys->url.psz_host,
                                             p_sys->url.i_port );
    if( fd == -1 )
    {
        msg_Err( p_access, "connection failed" );
        dialog_Fatal( p_access, _("Network interaction failed"), "%s",
                        _("VLC could not connect with the given server.") );
        return -1;
    }
    if ( p_sys->tlsmode == IMPLICIT ) /* FTPS Mode */
    {
        if ( createCmdTLS( p_access, p_sys, fd, "ftps") < 0 )
            goto error;
    }
    while( ftp_RecvCommand( p_access, p_sys, &i_answer, NULL ) == 1 );
    if( i_answer / 100 != 2 )
    {
        msg_Err( p_access, "connection rejected" );
        dialog_Fatal( p_access, _("Network interaction failed"), "%s",
                        _("VLC's connection to the given server was rejected.") );
        return -1;
    }
    msg_Dbg( p_access, "connection accepted (%d)", i_answer );
    /* Features check first */
    if( ftp_SendCommand( p_access, p_sys, "FEAT" ) < 0
     || ftp_RecvAnswer( p_access, p_sys, NULL, NULL,
                        FeaturesCheck, &p_sys->features ) < 0 )
    {
         msg_Err( p_access, "cannot get server features" );
         return -1;
    }
    /* Create TLS Session */
    if( p_sys->tlsmode == EXPLICIT )
    {
        if ( ! p_sys->features.b_authtls )
        {
            msg_Err( p_access, "Server does not support TLS" );
            return -1;
        }
        if( ftp_SendCommand( p_access, p_sys, "AUTH TLS" ) < 0
         || ftp_RecvCommand( p_access, p_sys, &i_answer, NULL ) < 0
         || i_answer != 234 )
        {
             msg_Err( p_access, "cannot switch to TLS: server replied with code %d",
                      i_answer );
             return -1;
        }
        if ( createCmdTLS( p_access, p_sys, fd, "ftpes") < 0 )
        {
            goto error;
        }
    }
    if( p_sys->tlsmode != NONE )
    {
        if( ftp_SendCommand( p_access, p_sys, "PBSZ 0" ) < 0 ||
            ftp_RecvCommand( p_access, p_sys, &i_answer, NULL ) < 0 ||
            i_answer != 200 )
        {
            msg_Err( p_access, "Can't truncate Protection buffer size for TLS" );
            goto error;
        }
        if( ftp_SendCommand( p_access, p_sys, "PROT P" ) < 0 ||
            ftp_RecvCommand( p_access, p_sys, &i_answer, NULL ) < 0 ||
            i_answer != 200 )
        {
            msg_Err( p_access, "Can't set Data channel protection" );
            goto error;
        }
    }
    /* Send credentials over channel */
    char *psz;
    if( p_sys->url.psz_username && *p_sys->url.psz_username )
        psz = strdup( p_sys->url.psz_username );
    else
        psz = var_InheritString( p_access, "ftp-user" );
    if( !psz )
        goto error;
    if( ftp_SendCommand( p_access, p_sys, "USER %s", psz ) < 0 ||
        ftp_RecvCommand( p_access, p_sys, &i_answer, NULL ) < 0 )
    {
        free( psz );
        goto error;
    }
    free( psz );
    switch( i_answer / 100 )
    {
        case 2:
            /* X.509 auth successful after AUTH TLS / RFC 2228 sec. 4 */
            if ( i_answer == 232 )
                msg_Dbg( p_access, "user accepted and authenticated" );
            else
                msg_Dbg( p_access, "user accepted" );
            break;
        case 3:
            msg_Dbg( p_access, "password needed" );
            if( p_sys->url.psz_password && *p_sys->url.psz_password )
                psz = strdup( p_sys->url.psz_password );
            else
                psz = var_InheritString( p_access, "ftp-pwd" );
            if( !psz )
                goto error;
            if( ftp_SendCommand( p_access, p_sys, "PASS %s", psz ) < 0 ||
                ftp_RecvCommand( p_access, p_sys, &i_answer, NULL ) < 0 )
            {
                free( psz );
                goto error;
            }
            free( psz );
            switch( i_answer / 100 )
            {
                case 2:
                    msg_Dbg( p_access, "password accepted" );
                    break;
                case 3:
                    msg_Dbg( p_access, "account needed" );
                    psz = var_InheritString( p_access, "ftp-account" );
                    if( ftp_SendCommand( p_access, p_sys, "ACCT %s",
                                         psz ) < 0 ||
                        ftp_RecvCommand( p_access, p_sys, &i_answer, NULL ) < 0 )
                    {
                        free( psz );
                        goto error;
                    }
                    free( psz );
                    if( i_answer / 100 != 2 )
                    {
                        msg_Err( p_access, "account rejected" );
                        dialog_Fatal( p_access,
                                      _("Network interaction failed"),
                                      "%s", _("Your account was rejected.") );
                        goto error;
                    }
                    msg_Dbg( p_access, "account accepted" );
                    break;
                default:
                    msg_Err( p_access, "password rejected" );
                    dialog_Fatal( p_access, _("Network interaction failed"),
                                  "%s",  _("Your password was rejected.") );
                    goto error;
            }
            break;
        default:
            msg_Err( p_access, "user rejected" );
            dialog_Fatal( p_access, _("Network interaction failed"), "%s",
                        _("Your connection attempt to the server was rejected.") );
            goto error;
    }
    return 0;
error:
    clearCmdTLS( p_sys );
    return -1;
}
