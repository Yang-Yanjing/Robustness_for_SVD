 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_access_out_t       *p_access = (sout_access_out_t*)p_this;
    sout_access_out_sys_t   *p_sys;
    char                *psz_user;
    char                *psz_pwd;
    char                *psz_mime;
    if( !( p_sys = p_access->p_sys =
                malloc( sizeof( sout_access_out_sys_t ) ) ) )
        return VLC_ENOMEM ;
    config_ChainParse( p_access, SOUT_CFG_PREFIX, ppsz_sout_options, p_access->p_cfg );
    const char *path = p_access->psz_path;
    path += strcspn( path, "/" );
    if( path > p_access->psz_path )
    {
        const char *port = strrchr( p_access->psz_path, ':' );
        if( port != NULL && strchr( port, ']' ) != NULL )
            port = NULL; /* IPv6 numeral */
        if( port != p_access->psz_path )
        {
            int len = (port ? port : path) - p_access->psz_path;
            msg_Warn( p_access, "\"%.*s\" HTTP host might be ignored in "
                      "multiple-host configurations, use at your own risks.",
                      len, p_access->psz_path );
            msg_Info( p_access, "Consider passing --http-host=IP on the "
                                "command line instead." );
            char host[len + 1];
            strncpy( host, p_access->psz_path, len );
            host[len] = '\0';
            var_Create( p_access, "http-host", VLC_VAR_STRING );
            var_SetString( p_access, "http-host", host );
        }
        if( port != NULL )
        {
            /* int len = path - ++port;
            msg_Info( p_access, "Consider passing --%s-port=%.*s on the "
                                "command line instead.",
                      strcasecmp( p_access->psz_access, "https" )
                      ? "http" : "https", len, port ); */
            port++;
            int bind_port = atoi( port );
            if( bind_port > 0 )
            {
                const char *var = strcasecmp( p_access->psz_access, "https" )
                                  ? "http-port" : "https-port";
                var_Create( p_access, var, VLC_VAR_INTEGER );
                var_SetInteger( p_access, var, bind_port );
            }
        }
    }
    if( !*path )
        path = "/";
    /* TLS support */
    if( p_access->psz_access && !strcmp( p_access->psz_access, "https" ) )
        p_sys->p_httpd_host = vlc_https_HostNew( VLC_OBJECT(p_access) );
    else
        p_sys->p_httpd_host = vlc_http_HostNew( VLC_OBJECT(p_access) );
    if( p_sys->p_httpd_host == NULL )
    {
        msg_Err( p_access, "cannot start HTTP server" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    psz_user = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "user" );
    psz_pwd = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "pwd" );
    if( p_access->psz_access && !strcmp( p_access->psz_access, "mmsh" ) )
    {
        psz_mime = strdup( "video/x-ms-asf-stream" );
    }
    else
    {
        psz_mime = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "mime" );
    }
    p_sys->b_metacube = var_GetBool( p_access, SOUT_CFG_PREFIX "metacube" );
    p_sys->b_has_keyframes = false;
    p_sys->p_httpd_stream =
        httpd_StreamNew( p_sys->p_httpd_host, path, psz_mime,
                         psz_user, psz_pwd );
    free( psz_user );
    free( psz_pwd );
    free( psz_mime );
    if( p_sys->p_httpd_stream == NULL )
    {
        msg_Err( p_access, "cannot add stream %s", path );
        httpd_HostDelete( p_sys->p_httpd_host );
        free( p_sys );
        return VLC_EGENERIC;
    }
    if( p_sys->b_metacube )
    {
        httpd_header headers[] = {{ "Content-encoding", "metacube" }};
        int err = httpd_StreamSetHTTPHeaders( p_sys->p_httpd_stream, headers, sizeof( headers ) / sizeof( httpd_header ) );
        if( err != VLC_SUCCESS )
        {
            free( p_sys );
            return err;
        }
    }
    p_sys->i_header_allocated = 1024;
    p_sys->i_header_size      = 0;
    p_sys->p_header           = xmalloc( p_sys->i_header_allocated );
    p_sys->b_header_complete  = false;
    p_access->pf_write       = Write;
    p_access->pf_seek        = Seek;
    p_access->pf_control     = Control;
    return VLC_SUCCESS;
}
