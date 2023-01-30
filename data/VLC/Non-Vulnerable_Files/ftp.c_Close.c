 *****************************************************************************/
static void Close( vlc_object_t *p_access, access_sys_t *p_sys )
{
    msg_Dbg( p_access, "stopping stream" );
    ftp_StopStream( p_access, p_sys );
    if( ftp_SendCommand( p_access, p_sys, "QUIT" ) < 0 )
    {
        msg_Warn( p_access, "cannot quit" );
    }
    else
    {
        ftp_RecvCommand( p_access, p_sys, NULL, NULL );
    }
    clearCmdTLS( p_sys );
    net_Close( p_sys->cmd.fd );
    /* free memory */
    vlc_UrlClean( &p_sys->url );
    free( p_sys );
}
