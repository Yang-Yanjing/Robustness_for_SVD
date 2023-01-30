 */
static int Open( vlc_object_t* p_this )
{
    access_t*   p_access = (access_t*)p_this;
    access_sys_t* p_sys;
    char* psz_username = NULL;
    char* psz_password = NULL;
    int i_port;
    int i_ret;
    vlc_url_t url;
    size_t i_len;
    int i_type;
    if( !p_access->psz_location )
        return VLC_EGENERIC;
    STANDARD_BLOCK_ACCESS_INIT;
    p_sys->i_socket = -1;
    /* Parse the URL */
    const char* path = p_access->psz_location;
    vlc_UrlParse( &url, path, 0 );
    /* Check for some parameters */
    if( EMPTY_STR( url.psz_host ) )
    {
        msg_Err( p_access, "You might give a non empty host" );
        goto error;
    }
    /* If the user name is empty, ask the user */
    if( !EMPTY_STR( url.psz_username ) && url.psz_password )
    {
        psz_username = strdup( url.psz_username );
        psz_password = strdup( url.psz_password );
    }
    else
    {
        dialog_Login( p_access, &psz_username, &psz_password,
                      _("SFTP authentication"),
                      _("Please enter a valid login and password for the sftp "
                        "connexion to %s"), url.psz_host );
        if( EMPTY_STR(psz_username) || !psz_password )
            goto error;
    }
    if( url.i_port <= 0 )
        i_port = var_InheritInteger( p_access, "sftp-port" );
    else
        i_port = url.i_port;
    /* Connect to the server using a regular socket */
    p_sys->i_socket = net_Connect( p_access, url.psz_host, i_port, SOCK_STREAM, 0 );
    if( p_sys->i_socket < 0 )
    {
        msg_Err( p_access, "Impossible to open the connection to %s:%i", url.psz_host, i_port );
        goto error;
    }
    /* Create the ssh connexion and wait until the server answer */
    if( ( p_sys->ssh_session = libssh2_session_init() ) == NULL )
        goto error;
    while( ( i_ret = libssh2_session_startup( p_sys->ssh_session,
                                              p_sys->i_socket ) )
           == LIBSSH2_ERROR_EAGAIN );
    if( i_ret != 0 )
    {
        msg_Err( p_access, "Impossible to open the connection to %s:%i", url.psz_host, i_port );
        goto error;
    }
    /* Set the socket in non-blocking mode */
    libssh2_session_set_blocking( p_sys->ssh_session, 1 );
    /* List the know hosts */
    LIBSSH2_KNOWNHOSTS *ssh_knownhosts = libssh2_knownhost_init( p_sys->ssh_session );
    if( !ssh_knownhosts )
        goto error;
    char *psz_home = config_GetUserDir( VLC_HOME_DIR );
    char *psz_knownhosts_file;
    if( asprintf( &psz_knownhosts_file, "%s/.ssh/known_hosts", psz_home ) != -1 )
    {
        libssh2_knownhost_readfile( ssh_knownhosts, psz_knownhosts_file,
                LIBSSH2_KNOWNHOST_FILE_OPENSSH );
        free( psz_knownhosts_file );
    }
    free( psz_home );
    const char *fingerprint = libssh2_session_hostkey( p_sys->ssh_session, &i_len, &i_type );
    struct libssh2_knownhost *host;
    int check = libssh2_knownhost_check( ssh_knownhosts, url.psz_host,
                                         fingerprint, i_len,
                                         LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                         LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                         &host );
    libssh2_knownhost_free( ssh_knownhosts );
    /* Check that it does match or at least that the host is unknown */
    switch(check)
    {
    case LIBSSH2_KNOWNHOST_CHECK_FAILURE:
    case LIBSSH2_KNOWNHOST_CHECK_NOTFOUND:
        msg_Dbg( p_access, "Unable to check the remote host" );
        break;
    case LIBSSH2_KNOWNHOST_CHECK_MATCH:
        msg_Dbg( p_access, "Succesfuly matched the host" );
        break;
    case LIBSSH2_KNOWNHOST_CHECK_MISMATCH:
        msg_Err( p_access, "The host does not match !! The remote key changed !!" );
        goto error;
    }
    //TODO: ask for the available auth methods
    /* send the login/password */
    if( libssh2_userauth_password( p_sys->ssh_session, psz_username, psz_password ) )
    {
        msg_Err( p_access, "Authentication by password failed" );
        goto error;
    }
    /* Create the sftp session */
    p_sys->sftp_session = libssh2_sftp_init( p_sys->ssh_session );
    if( !p_sys->sftp_session )
    {
        msg_Err( p_access, "Unable to initialize the SFTP session" );
        goto error;
    }
    /* Open the given file */
    p_sys->file = libssh2_sftp_open( p_sys->sftp_session, url.psz_path, LIBSSH2_FXF_READ, 0 );
    if( !p_sys->file )
    {
        msg_Err( p_access, "Unable to open the remote file %s", url.psz_path );
        goto error;
    }
    /* Get some information */
    LIBSSH2_SFTP_ATTRIBUTES attributes;
    if( libssh2_sftp_stat( p_sys->sftp_session, url.psz_path, &attributes ) )
    {
        msg_Err( p_access, "Impossible to get information about the remote file %s", url.psz_path );
        goto error;
    }
    p_sys->filesize = attributes.filesize;
    p_sys->i_read_size = var_InheritInteger( p_access, "sftp-readsize" );
    free( psz_password );
    free( psz_username );
    vlc_UrlClean( &url );
    return VLC_SUCCESS;
error:
    if( p_sys->file )
        libssh2_sftp_close_handle( p_sys->file );
    if( p_sys->ssh_session )
        libssh2_session_free( p_sys->ssh_session );
    free( psz_password );
    free( psz_username );
    vlc_UrlClean( &url );
    if( p_sys->i_socket >= 0 )
        net_Close( p_sys->i_socket );
    free( p_sys );
    return VLC_EGENERIC;
}
