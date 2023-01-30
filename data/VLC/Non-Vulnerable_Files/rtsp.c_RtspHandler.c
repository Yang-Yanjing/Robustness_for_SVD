 */
static int RtspHandler( rtsp_stream_t *rtsp, rtsp_stream_id_t *id,
                        httpd_client_t *cl,
                        httpd_message_t *answer,
                        const httpd_message_t *query )
{
    vlc_object_t *owner = rtsp->owner;
    char psz_sesbuf[17];
    const char *psz_session = NULL, *psz;
    char control[sizeof("rtsp://[]:12345") + NI_MAXNUMERICHOST
                  + strlen( rtsp->psz_path )];
    bool vod = rtsp->vod_media != NULL;
    time_t now;
    time (&now);
    if( answer == NULL || query == NULL || cl == NULL )
        return VLC_SUCCESS;
    else
    {
        /* Build self-referential control URL */
        char ip[NI_MAXNUMERICHOST], *ptr;
        int port;
        httpd_ServerIP( cl, ip, &port );
        ptr = strchr( ip, '%' );
        if( ptr != NULL )
            *ptr = '\0';
        if( strchr( ip, ':' ) != NULL )
            sprintf( control, "rtsp://[%s]:%d%s", ip, port, rtsp->psz_path );
        else
            sprintf( control, "rtsp://%s:%d%s", ip, port, rtsp->psz_path );
    }
    /* */
    answer->i_proto = HTTPD_PROTO_RTSP;
    answer->i_version= 0;
    answer->i_type   = HTTPD_MSG_ANSWER;
    answer->i_body = 0;
    answer->p_body = NULL;
    httpd_MsgAdd( answer, "Server", "VLC/%s", VERSION );
    /* Date: is always allowed, and sometimes mandatory with RTSP/2.0. */
    struct tm ut;
    if (gmtime_r (&now, &ut) != NULL)
    {   /* RFC1123 format, GMT is mandatory */
        static const char wdays[7][4] = {
            "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
        static const char mons[12][4] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        httpd_MsgAdd (answer, "Date", "%s, %02u %s %04u %02u:%02u:%02u GMT",
                      wdays[ut.tm_wday], ut.tm_mday, mons[ut.tm_mon],
                      1900 + ut.tm_year, ut.tm_hour, ut.tm_min, ut.tm_sec);
    }
    if( query->i_proto != HTTPD_PROTO_RTSP )
    {
        answer->i_status = 505;
    }
    else
    if( httpd_MsgGet( query, "Require" ) != NULL )
    {
        answer->i_status = 551;
        httpd_MsgAdd( answer, "Unsupported", "%s",
                      httpd_MsgGet( query, "Require" ) );
    }
    else
    switch( query->i_type )
    {
        case HTTPD_MSG_DESCRIBE:
        {   /* Aggregate-only */
            if( id != NULL )
            {
                answer->i_status = 460;
                break;
            }
            answer->i_status = 200;
            httpd_MsgAdd( answer, "Content-Type",  "%s", "application/sdp" );
            httpd_MsgAdd( answer, "Content-Base",  "%s", control );
            answer->p_body = (uint8_t *) ( vod ?
                SDPGenerateVoD( rtsp->vod_media, control ) :
                SDPGenerate( (sout_stream_t *)owner, control ) );
            if( answer->p_body != NULL )
                answer->i_body = strlen( (char *)answer->p_body );
            else
                answer->i_status = 500;
            break;
        }
        case HTTPD_MSG_SETUP:
            /* Non-aggregate-only */
            if( id == NULL )
            {
                answer->i_status = 459;
                break;
            }
            psz_session = httpd_MsgGet( query, "Session" );
            answer->i_status = 461;
            for( const char *tpt = httpd_MsgGet( query, "Transport" );
                 tpt != NULL;
                 tpt = transport_next( tpt ) )
            {
                bool b_multicast = true, b_unsupp = false;
                unsigned loport = 5004, hiport; /* from RFC3551 */
                /* Check transport protocol. */
                /* Currently, we only support RTP/AVP over UDP */
                if( strncmp( tpt, "RTP/AVP", 7 ) )
                    continue;
                tpt += 7;
                if( strncmp( tpt, "/UDP", 4 ) == 0 )
                    tpt += 4;
                if( strchr( ";,", *tpt ) == NULL )
                    continue;
                /* Parse transport options */
                for( const char *opt = parameter_next( tpt );
                     opt != NULL;
                     opt = parameter_next( opt ) )
                {
                    if( strncmp( opt, "multicast", 9 ) == 0)
                        b_multicast = true;
                    else
                    if( strncmp( opt, "unicast", 7 ) == 0 )
                        b_multicast = false;
                    else
                    if( sscanf( opt, "client_port=%u-%u", &loport, &hiport )
                                == 2 )
                        ;
                    else
                    if( strncmp( opt, "mode=", 5 ) == 0 )
                    {
                        if( strncasecmp( opt + 5, "play", 4 )
                         && strncasecmp( opt + 5, "\"PLAY\"", 6 ) )
                        {
                            /* Not playing?! */
                            b_unsupp = true;
                            break;
                        }
                    }
                    else
                    if( strncmp( opt,"destination=", 12 ) == 0 )
                    {
                        answer->i_status = 403;
                        b_unsupp = true;
                    }
                    else
                    {
                    /*
                     * Every other option is unsupported:
                     *
                     * "source" and "append" are invalid (server-only);
                     * "ssrc" also (as clarified per RFC2326bis).
                     *
                     * For multicast, "port", "layers", "ttl" are set by the
                     * stream output configuration.
                     *
                     * For unicast, we want to decide "server_port" values.
                     *
                     * "interleaved" is not implemented.
                     */
                        b_unsupp = true;
                        break;
                    }
                }
                if( b_unsupp )
                    continue;
                if( b_multicast )
                {
                    char dst[NI_MAXNUMERICHOST];
                    int dport, ttl;
                    if( id->mcast_fd == -1 )
                        continue;
                    net_GetPeerAddress(id->mcast_fd, dst, &dport);
                    ttl = var_InheritInteger(owner, "ttl");
                    if (ttl <= 0)
                    /* FIXME: the TTL is left to the OS default, we can
                     * only guess that it's 1. */
                        ttl = 1;
                    if( psz_session == NULL )
                    {
                        /* Create a dummy session ID */
                        snprintf( psz_sesbuf, sizeof( psz_sesbuf ), "%lu",
                                  vlc_mrand48() );
                        psz_session = psz_sesbuf;
                    }
                    answer->i_status = 200;
                    httpd_MsgAdd( answer, "Transport",
                                  "RTP/AVP/UDP;destination=%s;port=%u-%u;"
                                  "ttl=%d;mode=play",
                                  dst, dport, dport + 1, ttl );
                     /* FIXME: this doesn't work with RTP + RTCP mux */
                }
                else
                {
                    char ip[NI_MAXNUMERICHOST], src[NI_MAXNUMERICHOST];
                    rtsp_session_t *ses = NULL;
                    int fd, sport;
                    uint32_t ssrc;
                    if( httpd_ClientIP( cl, ip, NULL ) == NULL )
                    {
                        answer->i_status = 500;
                        continue;
                    }
                    fd = net_ConnectDgram( owner, ip, loport, -1,
                                           IPPROTO_UDP );
                    if( fd == -1 )
                    {
                        msg_Err( owner,
                                 "cannot create RTP socket for %s port %u",
                                 ip, loport );
                        answer->i_status = 500;
                        continue;
                    }
                    /* Ignore any unexpected incoming packet */
                    setsockopt (fd, SOL_SOCKET, SO_RCVBUF, &(int){ 0 },
                                sizeof (int));
                    net_GetSockAddress( fd, src, &sport );
                    vlc_mutex_lock( &rtsp->lock );
                    if( psz_session == NULL )
                    {
                        ses = RtspClientNew( rtsp );
                        snprintf( psz_sesbuf, sizeof( psz_sesbuf ), "%"PRIx64,
                                  ses->id );
                        psz_session = psz_sesbuf;
                    }
                    else
                    {
                        ses = RtspClientGet( rtsp, psz_session );
                        if( ses == NULL )
                        {
                            answer->i_status = 454;
                            vlc_mutex_unlock( &rtsp->lock );
                            net_Close( fd );
                            continue;
                        }
                    }
                    RtspClientAlive(ses);
                    rtsp_strack_t *tr = NULL;
                    for (int i = 0; i < ses->trackc; i++)
                    {
                        if (ses->trackv[i].id == id)
                        {
                            tr = ses->trackv + i;
                            break;
                        }
                    }
                    if (tr == NULL)
                    {
                        /* Set up a new track */
                        rtsp_strack_t track = { .id = id,
                                                .sout_id = id->sout_id,
                                                .setup_fd = fd,
                                                .rtp_fd = -1 };
                        if (vod)
                        {
                            vlc_rand_bytes (&track.seq_init,
                                            sizeof (track.seq_init));
                            vlc_rand_bytes (&track.ssrc, sizeof (track.ssrc));
                            ssrc = track.ssrc;
                        }
                        else
                            ssrc = id->ssrc;
                        INSERT_ELEM( ses->trackv, ses->trackc, ses->trackc,
                                     track );
                    }
                    else if (tr->setup_fd == -1)
                    {
                        /* The track was not SETUP, but it exists
                         * because there is a sout_id running for it */
                        tr->setup_fd = fd;
                        ssrc = tr->ssrc;
                    }
                    else
                    {
                        /* The track is already set up, and we don't
                         * support changing the transport parameters on
                         * the fly */
                        vlc_mutex_unlock( &rtsp->lock );
                        answer->i_status = 455;
                        net_Close( fd );
                        break;
                    }
                    vlc_mutex_unlock( &rtsp->lock );
                    httpd_ServerIP( cl, ip, NULL );
                    /* Specify source IP only if it is different from the
                     * RTSP control connection server address */
                    if( strcmp( src, ip ) )
                    {
                        char *ptr = strchr( src, '%' );
                        if( ptr != NULL ) *ptr = '\0'; /* remove scope ID */
                    }
                    else
                        src[0] = '\0';
                    httpd_MsgAdd( answer, "Transport",
                                  "RTP/AVP/UDP;unicast%s%s;"
                                  "client_port=%u-%u;server_port=%u-%u;"
                                  "ssrc=%08X;mode=play",
                                  src[0] ? ";source=" : "", src,
                                  loport, loport + 1, sport, sport + 1, ssrc );
                    answer->i_status = 200;
                }
                break;
            }
            break;
        case HTTPD_MSG_PLAY:
        {
            rtsp_session_t *ses;
            answer->i_status = 200;
            psz_session = httpd_MsgGet( query, "Session" );
            int64_t start = -1, end = -1, npt;
            const char *range = httpd_MsgGet (query, "Range");
            if (range != NULL)
            {
                if (strncmp (range, "npt=", 4))
                {
                    answer->i_status = 501;
                    break;
                }
                start = ParseNPT (range + 4);
                range = strchr(range, '-');
                if (range != NULL && *(range + 1))
                    end = ParseNPT (range + 1);
                if (end >= 0 && end < start)
                {
                    answer->i_status = 457;
                    break;
                }
                if (vod)
                {
                    if (vod_check_range(rtsp->vod_media, psz_session,
                                        start, end) != VLC_SUCCESS)
                    {
                        answer->i_status = 457;
                        break;
                    }
                }
                /* We accept start times of 0 even for broadcast streams
                 * that already started */
                else if (start > 0 || end >= 0)
                {
                    answer->i_status = 456;
                    break;
                }
            }
            vlc_mutex_lock( &rtsp->lock );
            ses = RtspClientGet( rtsp, psz_session );
            if( ses != NULL )
            {
                char info[ses->trackc * ( strlen( control ) + TRACK_PATH_SIZE
                          + sizeof("url=;seq=65535;rtptime=4294967295, ")
                                          - 1 ) + 1];
                size_t infolen = 0;
                RtspClientAlive(ses);
                sout_stream_id_sys_t *sout_id = NULL;
                if (vod)
                {
                    /* We don't keep a reference to the sout_stream_t,
                     * so we check if a sout_id is available instead. */
                    for (int i = 0; i < ses->trackc; i++)
                    {
                        sout_id = ses->trackv[i].sout_id;
                        if (sout_id != NULL)
                            break;
                    }
                }
                int64_t ts = rtp_get_ts(vod ? NULL : (sout_stream_t *)owner,
                                        sout_id, rtsp->vod_media, psz_session,
                                        vod ? NULL : &npt);
                for( int i = 0; i < ses->trackc; i++ )
                {
                    rtsp_strack_t *tr = ses->trackv + i;
                    if( ( id == NULL ) || ( tr->id == id ) )
                    {
                        if (tr->setup_fd == -1)
                            /* Track not SETUP */
                            continue;
                        uint16_t seq;
                        if( tr->rtp_fd == -1 )
                        {
                            /* Track not PLAYing yet */
                            if (tr->sout_id == NULL)
                                /* Instance not running yet (VoD) */
                                seq = tr->seq_init;
                            else
                            {
                                /* Instance running, add a sink to it */
                                tr->rtp_fd = dup_socket(tr->setup_fd);
                                if (tr->rtp_fd == -1)
                                    continue;
                                rtp_add_sink( tr->sout_id, tr->rtp_fd,
                                              false, &seq );
                            }
                        }
                        else
                        {
                            /* Track already playing */
                            assert( tr->sout_id != NULL );
                            seq = rtp_get_seq( tr->sout_id );
                        }
                        char *url = RtspAppendTrackPath( tr->id, control );
                        infolen += sprintf( info + infolen,
                                    "url=%s;seq=%u;rtptime=%u, ",
                                    url != NULL ? url : "", seq,
                                    rtp_compute_ts( tr->id->clock_rate, ts ) );
                        free( url );
                    }
                }
                if( infolen > 0 )
                {
                    info[infolen - 2] = '\0'; /* remove trailing ", " */
                    httpd_MsgAdd( answer, "RTP-Info", "%s", info );
                }
            }
            vlc_mutex_unlock( &rtsp->lock );
            if (ses != NULL)
            {
                if (vod)
                {
                    vod_play(rtsp->vod_media, psz_session, &start, end);
                    npt = start;
                }
                double f_npt = (double) npt / CLOCK_FREQ;
                httpd_MsgAdd( answer, "Range", "npt=%f-", f_npt );
            }
            if( httpd_MsgGet( query, "Scale" ) != NULL )
                httpd_MsgAdd( answer, "Scale", "1." );
            break;
        }
        case HTTPD_MSG_PAUSE:
        {
            if (id == NULL && !vod)
            {
                answer->i_status = 405;
                httpd_MsgAdd( answer, "Allow",
                              "DESCRIBE, TEARDOWN, PLAY, GET_PARAMETER" );
                break;
            }
            rtsp_session_t *ses;
            answer->i_status = 200;
            psz_session = httpd_MsgGet( query, "Session" );
            vlc_mutex_lock( &rtsp->lock );
            ses = RtspClientGet( rtsp, psz_session );
            if (ses != NULL)
            {
                if (id != NULL) /* "Mute" the selected track */
                {
                    bool found = false;
                    for (int i = 0; i < ses->trackc; i++)
                    {
                        rtsp_strack_t *tr = ses->trackv + i;;
                        if (tr->id == id)
                        {
                            if (tr->setup_fd == -1)
                                break;
                            found = true;
                            if (tr->rtp_fd != -1)
                            {
                                rtp_del_sink(tr->sout_id, tr->rtp_fd);
                                tr->rtp_fd = -1;
                            }
                            break;
                        }
                    }
                    if (!found)
                        answer->i_status = 455;
                }
                RtspClientAlive(ses);
            }
            vlc_mutex_unlock( &rtsp->lock );
            if (ses != NULL && id == NULL)
            {
                assert(vod);
                int64_t npt = 0;
                vod_pause(rtsp->vod_media, psz_session, &npt);
                double f_npt = (double) npt / CLOCK_FREQ;
                httpd_MsgAdd( answer, "Range", "npt=%f-", f_npt );
            }
            break;
        }
        case HTTPD_MSG_GETPARAMETER:
            if( query->i_body > 0 )
            {
                answer->i_status = 451;
                break;
            }
            psz_session = httpd_MsgGet( query, "Session" );
            answer->i_status = 200;
            vlc_mutex_lock( &rtsp->lock );
            rtsp_session_t *ses = RtspClientGet( rtsp, psz_session );
            if (ses != NULL)
                RtspClientAlive(ses);
            vlc_mutex_unlock( &rtsp->lock );
            break;
        case HTTPD_MSG_TEARDOWN:
        {
            rtsp_session_t *ses;
            answer->i_status = 200;
            psz_session = httpd_MsgGet( query, "Session" );
            vlc_mutex_lock( &rtsp->lock );
            ses = RtspClientGet( rtsp, psz_session );
            if( ses != NULL )
            {
                if( id == NULL ) /* Delete the entire session */
                {
                    RtspClientDel( rtsp, ses );
                    if (vod)
                        vod_stop(rtsp->vod_media, psz_session);
                    RtspUpdateTimer(rtsp);
                }
                else /* Delete one track from the session */
                {
                    for( int i = 0; i < ses->trackc; i++ )
                    {
                        if( ses->trackv[i].id == id )
                        {
                            RtspTrackClose( &ses->trackv[i] );
                            /* Keep VoD tracks whose instance is still
                             * running */
                            if (!(vod && ses->trackv[i].sout_id != NULL))
                                REMOVE_ELEM( ses->trackv, ses->trackc, i );
                        }
                    }
                    RtspClientAlive(ses);
                }
            }
            vlc_mutex_unlock( &rtsp->lock );
            break;
        }
        default:
            return VLC_EGENERIC;
    }
    if( psz_session )
    {
        if (rtsp->timeout > 0)
            httpd_MsgAdd( answer, "Session", "%s;timeout=%d", psz_session,
                                                              rtsp->timeout );
        else
            httpd_MsgAdd( answer, "Session", "%s", psz_session );
    }
    httpd_MsgAdd( answer, "Content-Length", "%d", answer->i_body );
    httpd_MsgAdd( answer, "Cache-Control", "no-cache" );
    psz = httpd_MsgGet( query, "Cseq" );
    if( psz != NULL )
        httpd_MsgAdd( answer, "Cseq", "%s", psz );
    psz = httpd_MsgGet( query, "Timestamp" );
    if( psz != NULL )
        httpd_MsgAdd( answer, "Timestamp", "%s", psz );
    return VLC_SUCCESS;
}
