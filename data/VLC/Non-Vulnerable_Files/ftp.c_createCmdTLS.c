}
static int createCmdTLS( vlc_object_t *p_access, access_sys_t *p_sys, int fd,
                         const char *psz_session_name )
{
    p_sys->p_creds = vlc_tls_ClientCreate( p_access );
    if( p_sys->p_creds == NULL ) return -1;
    /* TLS/SSL handshake */
    p_sys->cmd.p_tls = vlc_tls_ClientSessionCreate( p_sys->p_creds, fd,
                                                    p_sys->url.psz_host,
                                                    psz_session_name );
    if( p_sys->cmd.p_tls == NULL )
    {
        msg_Err( p_access, "cannot establish FTP/TLS session on command channel" );
        return -1;
    }
    p_sys->cmd.p_vs = &p_sys->cmd.p_tls->sock;
    return 0;
}
