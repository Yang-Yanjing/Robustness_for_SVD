 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_access_out_t *p_access = (sout_access_out_t*)p_this;
    sout_access_out_sys_t *p_sys;
    shout_t *p_shout;
    long i_ret;
    char *psz_val;
    char *psz_name;
    char *psz_description;
    char *psz_genre;
    char *psz_url;
    vlc_url_t url;
    config_ChainParse( p_access, SOUT_CFG_PREFIX, ppsz_sout_options, p_access->p_cfg );
    if( !p_access->psz_path )
    {
        msg_Err( p_access,
                 "please specify url=user:password@host:port/mountpoint" );
        return VLC_EGENERIC;
    }
    vlc_UrlParse( &url , p_access->psz_path, 0 );
    if( url.i_port <= 0 )
        url.i_port = 8000;
    p_sys = p_access->p_sys = malloc( sizeof( sout_access_out_sys_t ) );
    if( !p_sys )
    {
        vlc_UrlClean( &url );
        return VLC_ENOMEM;
    }
    psz_name = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "name" );
    psz_description = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "description" );
    psz_genre = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "genre" );
    psz_url = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "url" );
    p_shout = p_sys->p_shout = shout_new();
    if( !p_shout
         || shout_set_host( p_shout, url.psz_host ) != SHOUTERR_SUCCESS
         || shout_set_protocol( p_shout, SHOUT_PROTOCOL_ICY ) != SHOUTERR_SUCCESS
         || shout_set_port( p_shout, url.i_port ) != SHOUTERR_SUCCESS
         || shout_set_password( p_shout, url.psz_password ) != SHOUTERR_SUCCESS
         || shout_set_mount( p_shout, url.psz_path ) != SHOUTERR_SUCCESS
         || shout_set_user( p_shout, url.psz_username ) != SHOUTERR_SUCCESS
         || shout_set_agent( p_shout, "VLC media player " VERSION ) != SHOUTERR_SUCCESS
         || shout_set_name( p_shout, psz_name ) != SHOUTERR_SUCCESS
         || shout_set_description( p_shout, psz_description ) != SHOUTERR_SUCCESS
         || shout_set_genre( p_shout, psz_genre ) != SHOUTERR_SUCCESS
         || shout_set_url( p_shout, psz_url ) != SHOUTERR_SUCCESS
         /* || shout_set_nonblocking( p_shout, 1 ) != SHOUTERR_SUCCESS */
      )
    {
        msg_Err( p_access, "failed to initialize shout streaming to %s:%i/%s",
                 url.psz_host, url.i_port, url.psz_path );
        free( psz_name );
        free( psz_description );
        free( psz_genre );
        free( psz_url );
        goto error;
    }
    free( psz_name );
    free( psz_description );
    free( psz_genre );
    free( psz_url );
    i_ret = shout_set_format( p_shout, var_GetBool( p_access, SOUT_CFG_PREFIX "mp3" ) ?
                                       SHOUT_FORMAT_MP3 : SHOUT_FORMAT_OGG );
    if( i_ret != SHOUTERR_SUCCESS )
    {
        msg_Err( p_access, "failed to set the shoutcast streaming format" );
        goto error;
    }
    /* Don't force bitrate to 0 but only use when specified. This will otherwise
       show an empty field on icecast directory listing instead of NA */
    psz_val = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "bitrate" );
    if( psz_val )
    {
        i_ret = shout_set_audio_info( p_shout, SHOUT_AI_BITRATE, psz_val );
        free( psz_val );
        if( i_ret != SHOUTERR_SUCCESS )
        {
            msg_Err( p_access, "failed to set the information about the bitrate" );
            goto error;
        }
    }
    else
    {
        /* Bitrate information is used for icecast/shoutcast servers directory
           listings (sorting, stream info etc.) */
        msg_Warn( p_access, "no bitrate information specified (required for listing " \
                            "the server as public on the shoutcast website)" );
    }
    /* Information about samplerate, channels and quality will not be propagated
       through the YP protocol for icecast to the public directory listing when
       the icecast server is operating in shoutcast compatibility mode */
    psz_val = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "samplerate" );
    if( psz_val )
    {
        i_ret = shout_set_audio_info( p_shout, SHOUT_AI_SAMPLERATE, psz_val );
        free( psz_val );
        if( i_ret != SHOUTERR_SUCCESS )
        {
            msg_Err( p_access, "failed to set the information about the samplerate" );
            goto error;
        }
    }
    psz_val = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "channels" );
    if( psz_val )
    {
        i_ret = shout_set_audio_info( p_shout, SHOUT_AI_CHANNELS, psz_val );
        free( psz_val );
        if( i_ret != SHOUTERR_SUCCESS )
        {
            msg_Err( p_access, "failed to set the information about the number of channels" );
            goto error;
        }
    }
    psz_val = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "quality" );
    if( psz_val )
    {
        i_ret = shout_set_audio_info( p_shout, SHOUT_AI_QUALITY, psz_val );
        free( psz_val );
        if( i_ret != SHOUTERR_SUCCESS )
        {
            msg_Err( p_access, "failed to set the information about Ogg Vorbis quality" );
            goto error;
        }
    }
    if( var_GetBool( p_access, SOUT_CFG_PREFIX "public" ) )
    {
        i_ret = shout_set_public( p_shout, 1 );
        if( i_ret != SHOUTERR_SUCCESS )
        {
            msg_Err( p_access, "failed to set the server status setting to public" );
            goto error;
        }
    }
    /* Connect at startup. Cycle through the possible protocols. */
    i_ret = shout_get_connected( p_shout );
    while ( i_ret != SHOUTERR_CONNECTED )
    {
        /* Shout parameters cannot be changed on an open connection */
        i_ret = shout_close( p_shout );
        if( i_ret == SHOUTERR_SUCCESS )
        {
            i_ret = SHOUTERR_UNCONNECTED;
        }
        /* Re-initialize for Shoutcast using ICY protocol. Not needed for initial connection
           but it is when we are reconnecting after other protocol was tried. */
        i_ret = shout_set_protocol( p_shout, SHOUT_PROTOCOL_ICY );
        if( i_ret != SHOUTERR_SUCCESS )
        {
            msg_Err( p_access, "failed to set the protocol to 'icy'" );
            goto error;
        }
        i_ret = shout_open( p_shout );
        if( i_ret == SHOUTERR_SUCCESS )
        {
            i_ret = SHOUTERR_CONNECTED;
            msg_Dbg( p_access, "connected using 'icy' (shoutcast) protocol" );
        }
        else
        {
            msg_Warn( p_access, "failed to connect using 'icy' (shoutcast) protocol" );
            /* Shout parameters cannot be changed on an open connection */
            i_ret = shout_close( p_shout );
            if( i_ret == SHOUTERR_SUCCESS )
            {
                i_ret = SHOUTERR_UNCONNECTED;
            }
            /* IceCAST using HTTP protocol */
            i_ret = shout_set_protocol( p_shout, SHOUT_PROTOCOL_HTTP );
            if( i_ret != SHOUTERR_SUCCESS )
            {
                msg_Err( p_access, "failed to set the protocol to 'http'" );
                goto error;
            }
            i_ret = shout_open( p_shout );
            if( i_ret == SHOUTERR_SUCCESS )
            {
                i_ret = SHOUTERR_CONNECTED;
                msg_Dbg( p_access, "connected using 'http' (icecast 2.x) protocol" );
            }
            else
                msg_Warn( p_access, "failed to connect using 'http' (icecast 2.x) protocol " );
        }
/*
        for non-blocking, use:
        while( i_ret == SHOUTERR_BUSY )
        {
            sleep( 1 );
            i_ret = shout_get_connected( p_shout );
        }
*/
        if ( i_ret != SHOUTERR_CONNECTED )
        {
            msg_Warn( p_access, "unable to establish connection, retrying..." );
            msleep( 30000000 );
        }
    }
    if( i_ret != SHOUTERR_CONNECTED )
    {
        msg_Err( p_access, "failed to open shout stream to %s:%i/%s: %s",
                 url.psz_host, url.i_port, url.psz_path, shout_get_error(p_shout) );
        goto error;
    }
    p_access->pf_write = Write;
    p_access->pf_seek  = Seek;
    p_access->pf_control = Control;
    msg_Dbg( p_access, "shout access output opened (%s@%s:%i/%s)",
             url.psz_username, url.psz_host, url.i_port, url.psz_path );
    vlc_UrlClean( &url );
    return VLC_SUCCESS;
error:
    if( p_sys->p_shout )
        shout_free( p_sys->p_shout );
    vlc_UrlClean( &url );
    free( p_sys );
    return VLC_EGENERIC;
}
