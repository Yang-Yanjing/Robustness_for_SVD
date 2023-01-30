 ****************************************************************************/
static int InOpen( vlc_object_t *p_this )
{
    access_t     *p_access = (access_t*)p_this;
    access_sys_t *p_sys;
    char         *psz_arg;
    /* Init p_access */
    STANDARD_READ_ACCESS_INIT
    p_sys->data.fd = -1;
    p_sys->out = false;
    p_sys->directory = false;
    p_sys->size = 0;
    readTLSMode( p_sys, p_access->psz_access );
    if( parseURL( &p_sys->url, p_access->psz_location, p_sys->tlsmode ) )
        goto exit_error;
    if( Connect( p_this, p_sys ) )
        goto exit_error;
    /* get size */
    if( p_sys->url.psz_path == NULL )
        p_sys->directory = true;
    else
    if( ftp_SendCommand( p_this, p_sys, "SIZE %s", p_sys->url.psz_path ) < 0 )
        goto error;
    else
    if ( ftp_RecvCommand( p_this, p_sys, NULL, &psz_arg ) == 2 )
    {
        p_sys->size = atoll( &psz_arg[4] );
        free( psz_arg );
        msg_Dbg( p_access, "file size: %"PRIu64, p_sys->size );
    }
    else
    if( ftp_SendCommand( p_this, p_sys, "CWD %s", p_sys->url.psz_path ) < 0 )
        goto error;
    else
    if( ftp_RecvCommand( p_this, p_sys, NULL, NULL ) != 2 )
    {
        msg_Err( p_this, "file or directory does not exist" );
        goto error;
    }
    else
        p_sys->directory = true;
    /* Start the 'stream' */
    if( ftp_StartStream( p_this, p_sys, 0 ) < 0 )
    {
        msg_Err( p_this, "cannot retrieve file" );
        clearCmdTLS( p_sys );
        net_Close( p_sys->cmd.fd );
        goto exit_error;
    }
    return VLC_SUCCESS;
error:
    clearCmdTLS( p_sys );
    net_Close( p_sys->cmd.fd );
exit_error:
    vlc_UrlClean( &p_sys->url );
    free( p_sys );
    return VLC_EGENERIC;
}
