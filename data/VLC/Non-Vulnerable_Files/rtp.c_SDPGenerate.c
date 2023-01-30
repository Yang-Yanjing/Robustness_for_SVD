/*static*/
char *SDPGenerate( sout_stream_t *p_stream, const char *rtsp_url )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    char *psz_sdp = NULL;
    struct sockaddr_storage dst;
    socklen_t dstlen;
    int i;
    /*
     * When we have a fixed destination (typically when we do multicast),
     * we need to put the actual port numbers in the SDP.
     * When there is no fixed destination, we only support RTSP unicast
     * on-demand setup, so we should rather let the clients decide which ports
     * to use.
     * When there is both a fixed destination and RTSP unicast, we need to
     * put port numbers used by the fixed destination, otherwise the SDP would
     * become totally incorrect for multicast use. It should be noted that
     * port numbers from SDP with RTSP are only "recommendation" from the
     * server to the clients (per RFC2326), so only broken clients will fail
     * to handle this properly. There is no solution but to use two differents
     * output chain with two different RTSP URLs if you need to handle this
     * scenario.
     */
    int inclport;
    vlc_mutex_lock( &p_sys->lock_es );
    if( unlikely(p_sys->i_es == 0 || (rtsp_url != NULL && !p_sys->es[0]->rtsp_id)) )
        goto out; /* hmm... */
    if( p_sys->psz_destination != NULL )
    {
        inclport = 1;
        /* Oh boy, this is really ugly! */
        dstlen = sizeof( dst );
        if( p_sys->es[0]->listen.fd != NULL )
            getsockname( p_sys->es[0]->listen.fd[0],
                         (struct sockaddr *)&dst, &dstlen );
        else
            getpeername( p_sys->es[0]->sinkv[0].rtp_fd,
                         (struct sockaddr *)&dst, &dstlen );
    }
    else
    {
        inclport = 0;
        /* Check against URL format rtsp://[<ipv6>]:<port>/<path> */
        bool ipv6 = rtsp_url != NULL && strlen( rtsp_url ) > 7
                    && rtsp_url[7] == '[';
        /* Dummy destination address for RTSP */
        dstlen = ipv6 ? sizeof( struct sockaddr_in6 )
                      : sizeof( struct sockaddr_in );
        memset (&dst, 0, dstlen);
        dst.ss_family = ipv6 ? AF_INET6 : AF_INET;
#ifdef HAVE_SA_LEN
        dst.ss_len = dstlen;
#endif
    }
    psz_sdp = vlc_sdp_Start( VLC_OBJECT( p_stream ), SOUT_CFG_PREFIX,
                             NULL, 0, (struct sockaddr *)&dst, dstlen );
    if( psz_sdp == NULL )
        goto out;
    /* TODO: a=source-filter */
    if( p_sys->rtcp_mux )
        sdp_AddAttribute( &psz_sdp, "rtcp-mux", NULL );
    if( rtsp_url != NULL )
        sdp_AddAttribute ( &psz_sdp, "control", "%s", rtsp_url );
    const char *proto = "RTP/AVP"; /* protocol */
    if( rtsp_url == NULL )
    {
        switch( p_sys->proto )
        {
            case IPPROTO_UDP:
                break;
            case IPPROTO_TCP:
                proto = "TCP/RTP/AVP";
                break;
            case IPPROTO_DCCP:
                proto = "DCCP/RTP/AVP";
                break;
            case IPPROTO_UDPLITE:
                return psz_sdp;
        }
    }
    for( i = 0; i < p_sys->i_es; i++ )
    {
        sout_stream_id_sys_t *id = p_sys->es[i];
        rtp_format_t *rtp_fmt = &id->rtp_fmt;
        const char *mime_major; /* major MIME type */
        switch( rtp_fmt->cat )
        {
            case VIDEO_ES:
                mime_major = "video";
                break;
            case AUDIO_ES:
                mime_major = "audio";
                break;
            case SPU_ES:
                mime_major = "text";
                break;
            default:
                continue;
        }
        sdp_AddMedia( &psz_sdp, mime_major, proto, inclport * id->i_port,
                      rtp_fmt->payload_type, false, rtp_fmt->bitrate,
                      rtp_fmt->ptname, rtp_fmt->clock_rate, rtp_fmt->channels,
                      rtp_fmt->fmtp);
        /* cf RFC4566 §5.14 */
        if( inclport && !p_sys->rtcp_mux && (id->i_port & 1) )
            sdp_AddAttribute ( &psz_sdp, "rtcp", "%u", id->i_port + 1 );
        if( rtsp_url != NULL )
        {
            char *track_url = RtspAppendTrackPath( id->rtsp_id, rtsp_url );
            if( track_url != NULL )
            {
                sdp_AddAttribute ( &psz_sdp, "control", "%s", track_url );
                free( track_url );
            }
        }
        else
        {
            if( id->listen.fd != NULL )
                sdp_AddAttribute( &psz_sdp, "setup", "passive" );
            if( p_sys->proto == IPPROTO_DCCP )
                sdp_AddAttribute( &psz_sdp, "dccp-service-code",
                                  "SC:RTP%c",
                                  toupper( (unsigned char)mime_major[0] ) );
        }
    }
out:
    vlc_mutex_unlock( &p_sys->lock_es );
    return psz_sdp;
}
