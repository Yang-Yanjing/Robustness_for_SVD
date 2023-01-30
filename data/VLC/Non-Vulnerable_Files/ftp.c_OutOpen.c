#ifdef ENABLE_SOUT
static int OutOpen( vlc_object_t *p_this )
{
    sout_access_out_t *p_access = (sout_access_out_t *)p_this;
    access_sys_t      *p_sys;
    p_sys = calloc( 1, sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    /* Init p_access */
    p_sys->data.fd = -1;
    p_sys->out = true;
    readTLSMode( p_sys, p_access->psz_access );
    if( parseURL( &p_sys->url, p_access->psz_path, p_sys->tlsmode ) )
        goto exit_error;
    if( p_sys->url.psz_path == NULL )
    {
        msg_Err( p_this, "no filename specified" );
        goto exit_error;
    }
    if( Connect( p_this, p_sys ) )
        goto exit_error;
    /* Start the 'stream' */
    if( ftp_StartStream( p_this, p_sys, 0 ) < 0 )
    {
        msg_Err( p_access, "cannot store file" );
        clearCmdTLS( p_sys );
        net_Close( p_sys->cmd.fd );
        goto exit_error;
    }
    p_access->pf_seek = OutSeek;
    p_access->pf_write = Write;
    p_access->p_sys = (void *)p_sys;
    return VLC_SUCCESS;
exit_error:
    vlc_UrlClean( &p_sys->url );
    free( p_sys );
    return VLC_EGENERIC;
}
