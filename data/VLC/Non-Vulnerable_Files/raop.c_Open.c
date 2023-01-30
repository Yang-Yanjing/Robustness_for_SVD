 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys;
    char psz_local[NI_MAXNUMERICHOST];
    char *psz_pwfile = NULL;
    gcry_error_t i_gcrypt_err;
    int i_err = VLC_SUCCESS;
    uint32_t i_session_id;
    uint64_t i_client_instance;
    vlc_gcrypt_init();
    config_ChainParse( p_stream, SOUT_CFG_PREFIX, ppsz_sout_options,
                       p_stream->p_cfg );
    p_sys = calloc( 1, sizeof( *p_sys ) );
    if ( p_sys == NULL )
        return VLC_ENOMEM;
    p_stream->pf_add = Add;
    p_stream->pf_del = Del;
    p_stream->pf_send = Send;
    p_stream->p_sys = p_sys;
    p_stream->pace_nocontrol = true;
    p_sys->i_control_fd = -1;
    p_sys->i_stream_fd = -1;
    p_sys->i_volume = var_GetInteger( p_stream, SOUT_CFG_PREFIX "volume");
    p_sys->i_jack_type = JACK_TYPE_NONE;
    http_auth_Init( &p_sys->auth );
    p_sys->psz_host = var_GetNonEmptyString( p_stream,
                                             SOUT_CFG_PREFIX "host" );
    if ( p_sys->psz_host == NULL )
    {
        msg_Err( p_this, "Missing host" );
        i_err = VLC_EGENERIC;
        goto error;
    }
    p_sys->psz_password = var_GetNonEmptyString( p_stream,
                                                 SOUT_CFG_PREFIX "password" );
    if ( p_sys->psz_password == NULL )
    {
        /* Try password file instead */
        psz_pwfile = var_GetNonEmptyString( p_stream,
                                            SOUT_CFG_PREFIX "password-file" );
        if ( psz_pwfile != NULL )
        {
            p_sys->psz_password = ReadPasswordFile( p_this, psz_pwfile );
            if ( p_sys->psz_password == NULL )
            {
                i_err = VLC_EGENERIC;
                goto error;
            }
        }
    }
    if ( p_sys->psz_password != NULL )
        msg_Info( p_this, "Using password authentication" );
    var_AddCallback( p_stream, SOUT_CFG_PREFIX "volume",
                     VolumeCallback, NULL );
    p_sys->b_volume_callback = true;
    /* Open control connection */
    p_sys->i_control_fd = net_ConnectTCP( p_stream, p_sys->psz_host,
                                          RAOP_PORT );
    if ( p_sys->i_control_fd < 0 )
    {
        msg_Err( p_this, "Cannot establish control connection to %s:%d (%s)",
                 p_sys->psz_host, RAOP_PORT, vlc_strerror_c(errno) );
        i_err = VLC_EGENERIC;
        goto error;
    }
    /* Get local IP address */
    if ( net_GetSockAddress( p_sys->i_control_fd, psz_local, NULL ) )
    {
        msg_Err( p_this, "cannot get local IP address" );
        i_err = VLC_EGENERIC;
        goto error;
    }
    /* Random session ID */
    gcry_randomize( &i_session_id, sizeof( i_session_id ),
                    GCRY_STRONG_RANDOM );
    /* Random client instance */
    gcry_randomize( &i_client_instance, sizeof( i_client_instance ),
                    GCRY_STRONG_RANDOM );
    if ( asprintf( &p_sys->psz_client_instance, "%016"PRIX64,
                   i_client_instance ) < 0 )
    {
        i_err = VLC_ENOMEM;
        goto error;
    }
    /* Build session URL */
    if ( asprintf( &p_sys->psz_url, "rtsp://%s/%u",
                   psz_local, i_session_id ) < 0 )
    {
        i_err = VLC_ENOMEM;
        goto error;
    }
    /* Generate AES key and IV */
    gcry_randomize( p_sys->ps_aes_key, sizeof( p_sys->ps_aes_key ),
                    GCRY_STRONG_RANDOM );
    gcry_randomize( p_sys->ps_aes_iv, sizeof( p_sys->ps_aes_iv ),
                    GCRY_STRONG_RANDOM );
    /* Setup AES */
    i_gcrypt_err = gcry_cipher_open( &p_sys->aes_ctx, GCRY_CIPHER_AES,
                                     GCRY_CIPHER_MODE_CBC, 0 );
    if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
    {
        i_err = VLC_EGENERIC;
        goto error;
    }
    /* Set key */
    i_gcrypt_err = gcry_cipher_setkey( p_sys->aes_ctx, p_sys->ps_aes_key,
                                       sizeof( p_sys->ps_aes_key ) );
    if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
    {
        i_err = VLC_EGENERIC;
        goto error;
    }
    /* Protocol handshake */
    i_err = AnnounceSDP( p_this, psz_local, i_session_id );
    if ( i_err != VLC_SUCCESS )
        goto error;
    i_err = SendSetup( p_this );
    if ( i_err != VLC_SUCCESS )
        goto error;
    i_err = SendRecord( p_this );
    if ( i_err != VLC_SUCCESS )
        goto error;
    i_err = UpdateVolume( p_this );
    if ( i_err != VLC_SUCCESS )
        goto error;
    LogInfo( p_this );
    /* Open stream connection */
    p_sys->i_stream_fd = net_ConnectTCP( p_stream, p_sys->psz_host,
                                         p_sys->i_server_port );
    if ( p_sys->i_stream_fd < 0 )
    {
        msg_Err( p_this, "Cannot establish stream connection to %s:%d (%s)",
                 p_sys->psz_host, p_sys->i_server_port,
                 vlc_strerror_c(errno)  );
        i_err = VLC_EGENERIC;
        goto error;
    }
error:
    free( psz_pwfile );
    if ( i_err != VLC_SUCCESS )
        FreeSys( p_this, p_sys );
    return i_err;
}
