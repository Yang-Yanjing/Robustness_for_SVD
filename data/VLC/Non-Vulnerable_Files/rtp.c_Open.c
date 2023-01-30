 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t       *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t   *p_sys = NULL;
    config_chain_t      *p_cfg = NULL;
    char                *psz;
    bool          b_rtsp = false;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX,
                       ppsz_sout_options, p_stream->p_cfg );
    p_sys = malloc( sizeof( sout_stream_sys_t ) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->psz_destination = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "dst" );
    p_sys->i_port       = var_GetInteger( p_stream, SOUT_CFG_PREFIX "port" );
    p_sys->i_port_audio = var_GetInteger( p_stream, SOUT_CFG_PREFIX "port-audio" );
    p_sys->i_port_video = var_GetInteger( p_stream, SOUT_CFG_PREFIX "port-video" );
    p_sys->rtcp_mux     = var_GetBool( p_stream, SOUT_CFG_PREFIX "rtcp-mux" );
    if( p_sys->i_port_audio && p_sys->i_port_video == p_sys->i_port_audio )
    {
        msg_Err( p_stream, "audio and video RTP port must be distinct" );
        free( p_sys->psz_destination );
        free( p_sys );
        return VLC_EGENERIC;
    }
    for( p_cfg = p_stream->p_cfg; p_cfg != NULL; p_cfg = p_cfg->p_next )
    {
        if( !strcmp( p_cfg->psz_name, "sdp" )
         && ( p_cfg->psz_value != NULL )
         && !strncasecmp( p_cfg->psz_value, "rtsp:", 5 ) )
        {
            b_rtsp = true;
            break;
        }
    }
    if( !b_rtsp )
    {
        psz = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "sdp" );
        if( psz != NULL )
        {
            if( !strncasecmp( psz, "rtsp:", 5 ) )
                b_rtsp = true;
            free( psz );
        }
    }
    /* Transport protocol */
    p_sys->proto = IPPROTO_UDP;
    psz = var_GetNonEmptyString (p_stream, SOUT_CFG_PREFIX"proto");
    if ((psz == NULL) || !strcasecmp (psz, "udp"))
        (void)0; /* default */
    else
    if (!strcasecmp (psz, "dccp"))
    {
        p_sys->proto = IPPROTO_DCCP;
        p_sys->rtcp_mux = true; /* Force RTP/RTCP mux */
    }
#if 0
    else
    if (!strcasecmp (psz, "sctp"))
    {
        p_sys->proto = IPPROTO_TCP;
        p_sys->rtcp_mux = true; /* Force RTP/RTCP mux */
    }
#endif
#if 0
    else
    if (!strcasecmp (psz, "tcp"))
    {
        p_sys->proto = IPPROTO_TCP;
        p_sys->rtcp_mux = true; /* Force RTP/RTCP mux */
    }
#endif
    else
    if (!strcasecmp (psz, "udplite") || !strcasecmp (psz, "udp-lite"))
        p_sys->proto = IPPROTO_UDPLITE;
    else
        msg_Warn (p_this, "unknown or unsupported transport protocol \"%s\"",
                  psz);
    free (psz);
    var_Create (p_this, "dccp-service", VLC_VAR_STRING);
    p_sys->p_vod_media = NULL;
    p_sys->psz_vod_session = NULL;
    if (! strcmp(p_stream->psz_name, "vod"))
    {
        /* The VLM stops all instances before deleting a media, so this
         * reference will remain valid during the lifetime of the rtp
         * stream output. */
        p_sys->p_vod_media = var_InheritAddress(p_stream, "vod-media");
        if (p_sys->p_vod_media != NULL)
        {
            p_sys->psz_vod_session = var_InheritString(p_stream, "vod-session");
            if (p_sys->psz_vod_session == NULL)
            {
                msg_Err(p_stream, "missing VoD session");
                free(p_sys);
                return VLC_EGENERIC;
            }
            const char *mux = vod_get_mux(p_sys->p_vod_media);
            var_SetString(p_stream, SOUT_CFG_PREFIX "mux", mux);
        }
    }
    if( p_sys->psz_destination == NULL && !b_rtsp
        && p_sys->p_vod_media == NULL )
    {
        msg_Err( p_stream, "missing destination and not in RTSP mode" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    int i_ttl = var_GetInteger( p_stream, SOUT_CFG_PREFIX "ttl" );
    if( i_ttl != -1 )
    {
        var_Create( p_stream, "ttl", VLC_VAR_INTEGER );
        var_SetInteger( p_stream, "ttl", i_ttl );
    }
    p_sys->b_latm = var_GetBool( p_stream, SOUT_CFG_PREFIX "mp4a-latm" );
    /* NPT=0 time will be determined when we packetize the first packet
     * (of any ES). But we want to be able to report rtptime in RTSP
     * without waiting (and already did in the VoD case). So until then,
     * we use an arbitrary reference PTS for timestamp computations, and
     * then actual PTS will catch up using offsets. */
    p_sys->i_npt_zero = VLC_TS_INVALID;
    p_sys->i_pts_zero = rtp_init_ts(p_sys->p_vod_media,
                                    p_sys->psz_vod_session); 
    p_sys->i_es = 0;
    p_sys->es   = NULL;
    p_sys->rtsp = NULL;
    p_sys->psz_sdp = NULL;
    p_sys->b_export_sap = false;
    p_sys->p_session = NULL;
    p_sys->psz_sdp_file = NULL;
    p_sys->p_httpd_host = NULL;
    p_sys->p_httpd_file = NULL;
    p_stream->p_sys     = p_sys;
    vlc_mutex_init( &p_sys->lock_sdp );
    vlc_mutex_init( &p_sys->lock_ts );
    vlc_mutex_init( &p_sys->lock_es );
    psz = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "mux" );
    if( psz != NULL )
    {
        /* Check muxer type */
        if( strncasecmp( psz, "ps", 2 )
         && strncasecmp( psz, "mpeg1", 5 )
         && strncasecmp( psz, "ts", 2 ) )
        {
            msg_Err( p_stream, "unsupported muxer type for RTP (only TS/PS)" );
            free( psz );
            vlc_mutex_destroy( &p_sys->lock_sdp );
            vlc_mutex_destroy( &p_sys->lock_ts );
            vlc_mutex_destroy( &p_sys->lock_es );
            free( p_sys->psz_vod_session );
            free( p_sys->psz_destination );
            free( p_sys );
            return VLC_EGENERIC;
        }
        p_sys->p_grab = GrabberCreate( p_stream );
        p_sys->p_mux = sout_MuxNew( p_stream->p_sout, psz, p_sys->p_grab );
        free( psz );
        if( p_sys->p_mux == NULL )
        {
            msg_Err( p_stream, "cannot create muxer" );
            sout_AccessOutDelete( p_sys->p_grab );
            vlc_mutex_destroy( &p_sys->lock_sdp );
            vlc_mutex_destroy( &p_sys->lock_ts );
            vlc_mutex_destroy( &p_sys->lock_es );
            free( p_sys->psz_vod_session );
            free( p_sys->psz_destination );
            free( p_sys );
            return VLC_EGENERIC;
        }
        p_sys->packet = NULL;
        p_stream->pf_add  = MuxAdd;
        p_stream->pf_del  = MuxDel;
        p_stream->pf_send = MuxSend;
    }
    else
    {
        p_sys->p_mux    = NULL;
        p_sys->p_grab   = NULL;
        p_stream->pf_add    = Add;
        p_stream->pf_del    = Del;
        p_stream->pf_send   = Send;
    }
    p_stream->pace_nocontrol = true;
    if( var_GetBool( p_stream, SOUT_CFG_PREFIX"sap" ) )
        SDPHandleUrl( p_stream, "sap" );
    psz = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "sdp" );
    if( psz != NULL )
    {
        config_chain_t *p_cfg;
        SDPHandleUrl( p_stream, psz );
        for( p_cfg = p_stream->p_cfg; p_cfg != NULL; p_cfg = p_cfg->p_next )
        {
            if( !strcmp( p_cfg->psz_name, "sdp" ) )
            {
                if( p_cfg->psz_value == NULL || *p_cfg->psz_value == '\0' )
                    continue;
                /* needed both :sout-rtp-sdp= and rtp{sdp=} can be used */
                if( !strcmp( p_cfg->psz_value, psz ) )
                    continue;
                SDPHandleUrl( p_stream, p_cfg->psz_value );
            }
        }
        free( psz );
    }
    if( p_sys->p_mux != NULL )
    {
        sout_stream_id_sys_t *id = Add( p_stream, NULL );
        if( id == NULL )
        {
            Close( p_this );
            return VLC_EGENERIC;
        }
    }
    return VLC_SUCCESS;
}
