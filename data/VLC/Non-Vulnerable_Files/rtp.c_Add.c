/** Add an ES as a new RTP stream */
static sout_stream_id_sys_t *Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    /* NOTE: As a special case, if we use a non-RTP
     * mux (TS/PS), then p_fmt is NULL. */
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    char              *psz_sdp;
    sout_stream_id_sys_t *id = malloc( sizeof( *id ) );
    if( unlikely(id == NULL) )
        return NULL;
    id->p_stream   = p_stream;
    id->i_mtu = var_InheritInteger( p_stream, "mtu" );
    if( id->i_mtu <= 12 + 16 )
        id->i_mtu = 576 - 20 - 8; /* pessimistic */
    msg_Dbg( p_stream, "maximum RTP packet size: %d bytes", id->i_mtu );
#ifdef HAVE_SRTP
    id->srtp = NULL;
#endif
    vlc_mutex_init( &id->lock_sink );
    id->sinkc = 0;
    id->sinkv = NULL;
    id->rtsp_id = NULL;
    id->p_fifo = NULL;
    id->listen.fd = NULL;
    id->b_first_packet = true;
    id->i_caching =
        (int64_t)1000 * var_GetInteger( p_stream, SOUT_CFG_PREFIX "caching");
    vlc_rand_bytes (&id->i_sequence, sizeof (id->i_sequence));
    vlc_rand_bytes (id->ssrc, sizeof (id->ssrc));
    bool format = false;
    if (p_sys->p_vod_media != NULL)
    {
        id->rtp_fmt.ptname = NULL;
        uint32_t ssrc;
        int val = vod_init_id(p_sys->p_vod_media, p_sys->psz_vod_session,
                              p_fmt ? p_fmt->i_id : 0, id, &id->rtp_fmt,
                              &ssrc, &id->i_seq_sent_next);
        if (val == VLC_SUCCESS)
        {
            memcpy(id->ssrc, &ssrc, sizeof(id->ssrc));
            /* This is ugly, but id->i_seq_sent_next needs to be
             * initialized inside vod_init_id() to avoid race
             * conditions. */
            id->i_sequence = id->i_seq_sent_next;
        }
        /* vod_init_id() may fail either because the ES wasn't found in
         * the VoD media, or because the RTSP session is gone. In the
         * former case, id->rtp_fmt was left untouched. */
        format = (id->rtp_fmt.ptname != NULL);
    }
    if (!format)
    {
        id->rtp_fmt.fmtp = NULL; /* don't free() garbage on error */
        char *psz = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "mux" );
        if (p_fmt == NULL && psz == NULL)
            goto error;
        int val = rtp_get_fmt(VLC_OBJECT(p_stream), p_fmt, psz, &id->rtp_fmt);
        free( psz );
        if (val != VLC_SUCCESS)
            goto error;
    }
#ifdef HAVE_SRTP
    char *key = var_GetNonEmptyString (p_stream, SOUT_CFG_PREFIX"key");
    if (key)
    {
        vlc_gcrypt_init ();
        id->srtp = srtp_create (SRTP_ENCR_AES_CM, SRTP_AUTH_HMAC_SHA1, 10,
                                   SRTP_PRF_AES_CM, SRTP_RCC_MODE1);
        if (id->srtp == NULL)
        {
            free (key);
            goto error;
        }
        char *salt = var_GetNonEmptyString (p_stream, SOUT_CFG_PREFIX"salt");
        int val = srtp_setkeystring (id->srtp, key, salt ? salt : "");
        free (salt);
        free (key);
        if (val)
        {
            msg_Err (p_stream, "bad SRTP key/salt combination (%s)",
                     vlc_strerror_c(val));
            goto error;
        }
        id->i_sequence = 0; /* FIXME: awful hack for libvlc_srtp */
    }
#endif
    id->i_seq_sent_next = id->i_sequence;
    int mcast_fd = -1;
    if( p_sys->psz_destination != NULL )
    {
        /* Choose the port */
        uint16_t i_port = 0;
        if( p_fmt == NULL )
            ;
        else
        if( p_fmt->i_cat == AUDIO_ES && p_sys->i_port_audio > 0 )
            i_port = p_sys->i_port_audio;
        else
        if( p_fmt->i_cat == VIDEO_ES && p_sys->i_port_video > 0 )
            i_port = p_sys->i_port_video;
        /* We do not need the ES lock (p_sys->lock_es) here, because
         * this is the only one thread that can *modify* the ES table.
         * The ES lock protects the other threads from our modifications
         * (TAB_APPEND, TAB_REMOVE). */
        for (int i = 0; i_port && (i < p_sys->i_es); i++)
             if (i_port == p_sys->es[i]->i_port)
                 i_port = 0; /* Port already in use! */
        for (uint16_t p = p_sys->i_port; i_port == 0; p += 2)
        {
            if (p == 0)
            {
                msg_Err (p_stream, "too many RTP elementary streams");
                goto error;
            }
            i_port = p;
            for (int i = 0; i_port && (i < p_sys->i_es); i++)
                 if (p == p_sys->es[i]->i_port)
                     i_port = 0;
        }
        id->i_port = i_port;
        int type = SOCK_STREAM;
        switch( p_sys->proto )
        {
#ifdef SOCK_DCCP
            case IPPROTO_DCCP:
            {
                const char *code;
                switch (id->rtp_fmt.cat)
                {
                    case VIDEO_ES: code = "RTPV";     break;
                    case AUDIO_ES: code = "RTPARTPV"; break;
                    case SPU_ES:   code = "RTPTRTPV"; break;
                    default:       code = "RTPORTPV"; break;
                }
                var_SetString (p_stream, "dccp-service", code);
                type = SOCK_DCCP;
            }   /* fall through */
#endif
            case IPPROTO_TCP:
                id->listen.fd = net_Listen( VLC_OBJECT(p_stream),
                                            p_sys->psz_destination, i_port,
                                            type, p_sys->proto );
                if( id->listen.fd == NULL )
                {
                    msg_Err( p_stream, "passive COMEDIA RTP socket failed" );
                    goto error;
                }
                if( vlc_clone( &id->listen.thread, rtp_listen_thread, id,
                               VLC_THREAD_PRIORITY_LOW ) )
                {
                    net_ListenClose( id->listen.fd );
                    id->listen.fd = NULL;
                    goto error;
                }
                break;
            default:
            {
                int fd = net_ConnectDgram( p_stream, p_sys->psz_destination,
                                           i_port, -1, p_sys->proto );
                if( fd == -1 )
                {
                    msg_Err( p_stream, "cannot create RTP socket" );
                    goto error;
                }
                /* Ignore any unexpected incoming packet (including RTCP-RR
                 * packets in case of rtcp-mux) */
                setsockopt (fd, SOL_SOCKET, SO_RCVBUF, &(int){ 0 },
                            sizeof (int));
                rtp_add_sink( id, fd, p_sys->rtcp_mux, NULL );
                /* FIXME: test if this is multicast  */
                mcast_fd = fd;
            }
        }
    }
    if( p_fmt != NULL )
    switch( p_fmt->i_codec )
    {
        case VLC_CODEC_MULAW:
        case VLC_CODEC_ALAW:
        case VLC_CODEC_U8:
            rtp_set_ptime (id, 20, 1);
            break;
        case VLC_CODEC_S16B:
        case VLC_CODEC_S16L:
            rtp_set_ptime (id, 20, 2);
            break;
        case VLC_CODEC_S24B:
            rtp_set_ptime (id, 20, 3);
            break;
        default:
            break;
    }
#if 0 /* No payload formats sets this at the moment */
    int cscov = -1;
    if( cscov != -1 )
        cscov += 8 /* UDP */ + 12 /* RTP */;
    if( id->sinkc > 0 )
        net_SetCSCov( id->sinkv[0].rtp_fd, cscov, -1 );
#endif
    vlc_mutex_lock( &p_sys->lock_ts );
    id->b_ts_init = ( p_sys->i_npt_zero != VLC_TS_INVALID );
    vlc_mutex_unlock( &p_sys->lock_ts );
    if( id->b_ts_init )
        id->i_ts_offset = rtp_compute_ts( id->rtp_fmt.clock_rate,
                                          p_sys->i_pts_offset );
    if( p_sys->rtsp != NULL )
        id->rtsp_id = RtspAddId( p_sys->rtsp, id, GetDWBE( id->ssrc ),
                                 id->rtp_fmt.clock_rate, mcast_fd );
    id->p_fifo = block_FifoNew();
    if( unlikely(id->p_fifo == NULL) )
        goto error;
    if( vlc_clone( &id->thread, ThreadSend, id, VLC_THREAD_PRIORITY_HIGHEST ) )
    {
        block_FifoRelease( id->p_fifo );
        id->p_fifo = NULL;
        goto error;
    }
    /* Update p_sys context */
    vlc_mutex_lock( &p_sys->lock_es );
    TAB_APPEND( p_sys->i_es, p_sys->es, id );
    vlc_mutex_unlock( &p_sys->lock_es );
    psz_sdp = SDPGenerate( p_stream, NULL );
    vlc_mutex_lock( &p_sys->lock_sdp );
    free( p_sys->psz_sdp );
    p_sys->psz_sdp = psz_sdp;
    vlc_mutex_unlock( &p_sys->lock_sdp );
    msg_Dbg( p_stream, "sdp=\n%s", p_sys->psz_sdp );
    /* Update SDP (sap/file) */
    if( p_sys->b_export_sap ) SapSetup( p_stream );
    if( p_sys->psz_sdp_file != NULL ) FileSetup( p_stream );
    return id;
error:
    Del( p_stream, id );
    return NULL;
}
