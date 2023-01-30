}
static int OpenConnection( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    vlc_url_t    srv = p_sys->b_proxy ? p_sys->proxy : p_sys->url;
    if( ( p_sys->fd = net_ConnectTCP( p_access,
                                      srv.psz_host, srv.i_port ) ) < 0 )
    {
        msg_Err( p_access, "cannot connect to %s:%d",
                 srv.psz_host, srv.i_port );
        return VLC_EGENERIC;
    }
    if( p_sys->b_proxy )
    {
        net_Printf( p_access, p_sys->fd, NULL,
                    "GET http://%s:%d%s HTTP/1.0\r\n",
                    p_sys->url.psz_host, p_sys->url.i_port,
                    ( (p_sys->url.psz_path == NULL) ||
                      (*p_sys->url.psz_path == '\0') ) ?
                         "/" : p_sys->url.psz_path );
        /* Proxy Authentication */
        if( p_sys->proxy.psz_username && *p_sys->proxy.psz_username )
        {
            char *buf;
            char *b64;
            if( asprintf( &buf, "%s:%s", p_sys->proxy.psz_username,
                       p_sys->proxy.psz_password ? p_sys->proxy.psz_password : "" ) == -1 )
                return VLC_ENOMEM;
            b64 = vlc_b64_encode( buf );
            free( buf );
            net_Printf( p_access, p_sys->fd, NULL,
                        "Proxy-Authorization: Basic %s\r\n", b64 );
            free( b64 );
        }
    }
    else
    {
        net_Printf( p_access, p_sys->fd, NULL,
                    "GET %s HTTP/1.0\r\n"
                    "Host: %s:%d\r\n",
                    ( (p_sys->url.psz_path == NULL) ||
                      (*p_sys->url.psz_path == '\0') ) ?
                            "/" : p_sys->url.psz_path,
                    p_sys->url.psz_host, p_sys->url.i_port );
    }
    return VLC_SUCCESS;
}
