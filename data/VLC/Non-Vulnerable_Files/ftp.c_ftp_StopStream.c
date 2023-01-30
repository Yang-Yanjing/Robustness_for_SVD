}
static int ftp_StopStream ( vlc_object_t *p_access, access_sys_t *p_sys )
{
    if( ftp_SendCommand( p_access, p_sys, "ABOR" ) < 0 )
    {
        msg_Warn( p_access, "cannot abort file" );
        if( p_sys->data.fd > 0 )
        {
            if ( p_sys->data.p_tls ) vlc_tls_SessionDelete( p_sys->data.p_tls );
            net_Close( p_sys->data.fd );
        }
        p_sys->data.fd = -1;
        p_sys->data.p_tls = NULL;
        p_sys->data.p_vs = NULL;
        return VLC_EGENERIC;
    }
    if( p_sys->data.fd != -1 )
    {
        if ( p_sys->data.p_tls ) vlc_tls_SessionDelete( p_sys->data.p_tls );
        net_Close( p_sys->data.fd );
        p_sys->data.fd = -1;
        p_sys->data.p_tls = NULL;
        p_sys->data.p_vs = NULL;
        /* Read the final response from RETR/STOR, i.e. 426 or 226 */
        ftp_RecvCommand( p_access, p_sys, NULL, NULL );
    }
    /* Read the response from ABOR, i.e. 226 or 225 */
    ftp_RecvCommand( p_access, p_sys, NULL, NULL );
    return VLC_SUCCESS;
}
