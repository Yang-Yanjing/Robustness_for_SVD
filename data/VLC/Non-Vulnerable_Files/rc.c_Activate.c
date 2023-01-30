 *****************************************************************************/
static int Activate( vlc_object_t *p_this )
{
    /* FIXME: This function is full of memory leaks and bugs in error paths. */
    intf_thread_t *p_intf = (intf_thread_t*)p_this;
    playlist_t *p_playlist = pl_Get( p_intf );
    char *psz_host, *psz_unix_path = NULL;
    int  *pi_socket = NULL;
#ifndef _WIN32
#if defined(HAVE_ISATTY)
    /* Check that stdin is a TTY */
    if( !var_InheritBool( p_intf, "rc-fake-tty" ) && !isatty( 0 ) )
    {
        msg_Warn( p_intf, "fd 0 is not a TTY" );
        return VLC_EGENERIC;
    }
#endif
    psz_unix_path = var_InheritString( p_intf, "rc-unix" );
    if( psz_unix_path )
    {
        int i_socket;
#ifndef AF_LOCAL
        msg_Warn( p_intf, "your OS doesn't support filesystem sockets" );
        free( psz_unix_path );
        return VLC_EGENERIC;
#else
        struct sockaddr_un addr;
        memset( &addr, 0, sizeof(struct sockaddr_un) );
        msg_Dbg( p_intf, "trying UNIX socket" );
        if( (i_socket = vlc_socket( PF_LOCAL, SOCK_STREAM, 0, false ) ) < 0 )
        {
            msg_Warn( p_intf, "can't open socket: %s", vlc_strerror_c(errno) );
            free( psz_unix_path );
            return VLC_EGENERIC;
        }
        addr.sun_family = AF_LOCAL;
        strncpy( addr.sun_path, psz_unix_path, sizeof( addr.sun_path ) );
        addr.sun_path[sizeof( addr.sun_path ) - 1] = '\0';
        if (bind (i_socket, (struct sockaddr *)&addr, sizeof (addr))
         && (errno == EADDRINUSE)
         && connect (i_socket, (struct sockaddr *)&addr, sizeof (addr))
         && (errno == ECONNREFUSED))
        {
            msg_Info (p_intf, "Removing dead UNIX socket: %s", psz_unix_path);
            unlink (psz_unix_path);
            if (bind (i_socket, (struct sockaddr *)&addr, sizeof (addr)))
            {
                msg_Err (p_intf, "cannot bind UNIX socket at %s: %s",
                         psz_unix_path, vlc_strerror_c(errno));
                free (psz_unix_path);
                net_Close (i_socket);
                return VLC_EGENERIC;
            }
        }
        if( listen( i_socket, 1 ) )
        {
            msg_Warn (p_intf, "can't listen on socket: %s",
                      vlc_strerror_c(errno));
            free( psz_unix_path );
            net_Close( i_socket );
            return VLC_EGENERIC;
        }
        /* FIXME: we need a core function to merge listening sockets sets */
        pi_socket = calloc( 2, sizeof( int ) );
        if( pi_socket == NULL )
        {
            free( psz_unix_path );
            net_Close( i_socket );
            return VLC_ENOMEM;
        }
        pi_socket[0] = i_socket;
        pi_socket[1] = -1;
#endif /* AF_LOCAL */
    }
#endif /* !_WIN32 */
    if( ( pi_socket == NULL ) &&
        ( psz_host = var_InheritString( p_intf, "rc-host" ) ) != NULL )
    {
        vlc_url_t url;
        vlc_UrlParse( &url, psz_host, 0 );
        msg_Dbg( p_intf, "base: %s, port: %d", url.psz_host, url.i_port );
        pi_socket = net_ListenTCP(p_this, url.psz_host, url.i_port);
        if( pi_socket == NULL )
        {
            msg_Warn( p_intf, "can't listen to %s port %i",
                      url.psz_host, url.i_port );
            vlc_UrlClean( &url );
            free( psz_host );
            return VLC_EGENERIC;
        }
        vlc_UrlClean( &url );
        free( psz_host );
    }
    intf_sys_t *p_sys = malloc( sizeof( *p_sys ) );
    if( unlikely(p_sys == NULL) )
    {
        net_ListenClose( pi_socket );
        free( psz_unix_path );
        return VLC_ENOMEM;
    }
    p_intf->p_sys = p_sys;
    p_sys->pi_socket_listen = pi_socket;
    p_sys->i_socket = -1;
    p_sys->psz_unix_path = psz_unix_path;
    vlc_mutex_init( &p_sys->status_lock );
    p_sys->i_last_state = PLAYLIST_STOPPED;
    p_sys->b_input_buffering = false;
    p_sys->p_playlist = p_playlist;
    p_sys->p_input = NULL;
    /* Non-buffered stdout */
    setvbuf( stdout, (char *)NULL, _IOLBF, 0 );
#ifdef _WIN32
    p_sys->b_quiet = var_InheritBool( p_intf, "rc-quiet" );
    if( !p_sys->b_quiet )
#endif
    {
        CONSOLE_INTRO_MSG;
    }
    if( vlc_clone( &p_sys->thread, Run, p_intf, VLC_THREAD_PRIORITY_LOW ) )
        abort();
    msg_rc( "%s", _("Remote control interface initialized. Type `help' for help.") );
    /* Listen to audio volume updates */
    var_AddCallback( p_sys->p_playlist, "volume", VolumeChanged, p_intf );
    return VLC_SUCCESS;
}
