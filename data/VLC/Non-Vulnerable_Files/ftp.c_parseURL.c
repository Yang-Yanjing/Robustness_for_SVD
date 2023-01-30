}
static int parseURL( vlc_url_t *url, const char *path, enum tls_mode_e mode )
{
    if( path == NULL )
        return VLC_EGENERIC;
    /* *** Parse URL and get server addr/port and path *** */
    while( *path == '/' )
        path++;
    vlc_UrlParse( url, path, 0 );
    if( url->psz_host == NULL || *url->psz_host == '\0' )
        return VLC_EGENERIC;
    if( url->i_port <= 0 )
    {
        if( mode == IMPLICIT )
            url->i_port = IPPORT_FTPS;
        else
            url->i_port = IPPORT_FTP; /* default port */
    }
    if( url->psz_path == NULL )
        return VLC_SUCCESS;
    /* FTP URLs are relative to user's default directory (RFC1738 §3.2)
    For absolute path use ftp://foo.bar//usr/local/etc/filename */
    /* FIXME: we should issue a series of CWD, one per slash */
    if( url->psz_path )
    {
        assert( url->psz_path[0] == '/' );
        url->psz_path++;
    }
    char *type = strstr( url->psz_path, ";type=" );
    if( type )
    {
        *type = '\0';
        if( strchr( "iI", type[6] ) == NULL )
            return VLC_EGENERIC; /* ASCII and directory not supported */
    }
    decode_URI( url->psz_path );
    return VLC_SUCCESS;
}
