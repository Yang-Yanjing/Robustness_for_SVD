int ff_rtsp_connect(AVFormatContext *s)
{
    RTSPState *rt = s->priv_data;
    char proto[128], host[1024], path[1024];
    char tcpname[1024], cmd[2048], auth[128];
    const char *lower_rtsp_proto = "tcp";
    int port, err, tcp_fd;
    RTSPMessageHeader reply1 = {0}, *reply = &reply1;
    int lower_transport_mask = 0;
    int default_port = RTSP_DEFAULT_PORT;
    char real_challenge[64] = "";
    struct sockaddr_storage peer;
    socklen_t peer_len = sizeof(peer);
    if (rt->rtp_port_max < rt->rtp_port_min) {
        av_log(s, AV_LOG_ERROR, "Invalid UDP port range, max port %d less "
                                "than min port %d\n", rt->rtp_port_max,
                                                      rt->rtp_port_min);
        return AVERROR(EINVAL);
    }
    if (!ff_network_init())
        return AVERROR(EIO);
    if (s->max_delay < 0) 
        s->max_delay = s->iformat ? DEFAULT_REORDERING_DELAY : 0;
    rt->control_transport = RTSP_MODE_PLAIN;
    if (rt->lower_transport_mask & (1 << RTSP_LOWER_TRANSPORT_HTTP)) {
        rt->lower_transport_mask = 1 << RTSP_LOWER_TRANSPORT_TCP;
        rt->control_transport = RTSP_MODE_TUNNEL;
    }
    
    rt->lower_transport_mask &= (1 << RTSP_LOWER_TRANSPORT_NB) - 1;
redirect:
    
    av_url_split(proto, sizeof(proto), auth, sizeof(auth),
                 host, sizeof(host), &port, path, sizeof(path), s->filename);
    if (!strcmp(proto, "rtsps")) {
        lower_rtsp_proto         = "tls";
        default_port             = RTSPS_DEFAULT_PORT;
        rt->lower_transport_mask = 1 << RTSP_LOWER_TRANSPORT_TCP;
    }
    if (*auth) {
        av_strlcpy(rt->auth, auth, sizeof(rt->auth));
    }
    if (port < 0)
        port = default_port;
    lower_transport_mask = rt->lower_transport_mask;
    if (!lower_transport_mask)
        lower_transport_mask = (1 << RTSP_LOWER_TRANSPORT_NB) - 1;
    if (s->oformat) {
        
        lower_transport_mask &= (1 << RTSP_LOWER_TRANSPORT_UDP) |
                                (1 << RTSP_LOWER_TRANSPORT_TCP);
        if (!lower_transport_mask || rt->control_transport == RTSP_MODE_TUNNEL) {
            av_log(s, AV_LOG_ERROR, "Unsupported lower transport method, "
                                    "only UDP and TCP are supported for output.\n");
            err = AVERROR(EINVAL);
            goto fail;
        }
    }
    


    ff_url_join(rt->control_uri, sizeof(rt->control_uri), proto, NULL,
                host, port, "%s", path);
    if (rt->control_transport == RTSP_MODE_TUNNEL) {
        
        char httpname[1024];
        char sessioncookie[17];
        char headers[1024];
        ff_url_join(httpname, sizeof(httpname), "http", auth, host, port, "%s", path);
        snprintf(sessioncookie, sizeof(sessioncookie), "%08x%08x",
                 av_get_random_seed(), av_get_random_seed());
        
        if (ffurl_alloc(&rt->rtsp_hd, httpname, AVIO_FLAG_READ,
                        &s->interrupt_callback) < 0) {
            err = AVERROR(EIO);
            goto fail;
        }
        
        snprintf(headers, sizeof(headers),
                 "x-sessioncookie: %s\r\n"
                 "Accept: application/x-rtsp-tunnelled\r\n"
                 "Pragma: no-cache\r\n"
                 "Cache-Control: no-cache\r\n",
                 sessioncookie);
        av_opt_set(rt->rtsp_hd->priv_data, "headers", headers, 0);
        
        if (ffurl_connect(rt->rtsp_hd, NULL)) {
            err = AVERROR(EIO);
            goto fail;
        }
        
        if (ffurl_alloc(&rt->rtsp_hd_out, httpname, AVIO_FLAG_WRITE,
                        &s->interrupt_callback) < 0 ) {
            err = AVERROR(EIO);
            goto fail;
        }
        
        snprintf(headers, sizeof(headers),
                 "x-sessioncookie: %s\r\n"
                 "Content-Type: application/x-rtsp-tunnelled\r\n"
                 "Pragma: no-cache\r\n"
                 "Cache-Control: no-cache\r\n"
                 "Content-Length: 32767\r\n"
                 "Expires: Sun, 9 Jan 1972 00:00:00 GMT\r\n",
                 sessioncookie);
        av_opt_set(rt->rtsp_hd_out->priv_data, "headers", headers, 0);
        av_opt_set(rt->rtsp_hd_out->priv_data, "chunked_post", "0", 0);
        















        ff_http_init_auth_state(rt->rtsp_hd_out, rt->rtsp_hd);
        
        if (ffurl_connect(rt->rtsp_hd_out, NULL)) {
            err = AVERROR(EIO);
            goto fail;
        }
    } else {
        int ret;
        
        ff_url_join(tcpname, sizeof(tcpname), lower_rtsp_proto, NULL,
                    host, port,
                    "?timeout=%d", rt->stimeout);
        if ((ret = ffurl_open(&rt->rtsp_hd, tcpname, AVIO_FLAG_READ_WRITE,
                       &s->interrupt_callback, NULL)) < 0) {
            err = ret;
            goto fail;
        }
        rt->rtsp_hd_out = rt->rtsp_hd;
    }
    rt->seq = 0;
    tcp_fd = ffurl_get_file_handle(rt->rtsp_hd);
    if (tcp_fd < 0) {
        err = tcp_fd;
        goto fail;
    }
    if (!getpeername(tcp_fd, (struct sockaddr*) &peer, &peer_len)) {
        getnameinfo((struct sockaddr*) &peer, peer_len, host, sizeof(host),
                    NULL, 0, NI_NUMERICHOST);
    }
    

    for (rt->server_type = RTSP_SERVER_RTP;;) {
        cmd[0] = 0;
        if (rt->server_type == RTSP_SERVER_REAL)
            av_strlcat(cmd,
                       








                       "ClientChallenge: 9e26d33f2984236010ef6253fb1887f7\r\n"
                       "PlayerStarttime: [28/03/2003:22:50:23 00:00]\r\n"
                       "CompanyID: KnKV4M4I/B2FjJ1TToLycw==\r\n"
                       "GUID: 00000000-0000-0000-0000-000000000000\r\n",
                       sizeof(cmd));
        ff_rtsp_send_cmd(s, "OPTIONS", rt->control_uri, cmd, reply, NULL);
        if (reply->status_code != RTSP_STATUS_OK) {
            err = ff_rtsp_averror(reply->status_code, AVERROR_INVALIDDATA);
            goto fail;
        }
        
        if (rt->server_type != RTSP_SERVER_REAL && reply->real_challenge[0]) {
            rt->server_type = RTSP_SERVER_REAL;
            continue;
        } else if (!av_strncasecmp(reply->server, "WMServer/", 9)) {
            rt->server_type = RTSP_SERVER_WMS;
        } else if (rt->server_type == RTSP_SERVER_REAL)
            strcpy(real_challenge, reply->real_challenge);
        break;
    }
    if (CONFIG_RTSP_DEMUXER && s->iformat)
        err = ff_rtsp_setup_input_streams(s, reply);
    else if (CONFIG_RTSP_MUXER)
        err = ff_rtsp_setup_output_streams(s, host);
    else
        av_assert0(0);
    if (err)
        goto fail;
    do {
        int lower_transport = ff_log2_tab[lower_transport_mask &
                                  ~(lower_transport_mask - 1)];
        if ((lower_transport_mask & (1 << RTSP_LOWER_TRANSPORT_TCP))
                && (rt->rtsp_flags & RTSP_FLAG_PREFER_TCP))
            lower_transport = RTSP_LOWER_TRANSPORT_TCP;
        err = ff_rtsp_make_setup_request(s, host, port, lower_transport,
                                 rt->server_type == RTSP_SERVER_REAL ?
                                     real_challenge : NULL);
        if (err < 0)
            goto fail;
        lower_transport_mask &= ~(1 << lower_transport);
        if (lower_transport_mask == 0 && err == 1) {
            err = AVERROR(EPROTONOSUPPORT);
            goto fail;
        }
    } while (err);
    rt->lower_transport_mask = lower_transport_mask;
    av_strlcpy(rt->real_challenge, real_challenge, sizeof(rt->real_challenge));
    rt->state = RTSP_STATE_IDLE;
    rt->seek_timestamp = 0; 
    return 0;
 fail:
    ff_rtsp_close_streams(s);
    ff_rtsp_close_connections(s);
    if (reply->status_code >=300 && reply->status_code < 400 && s->iformat) {
        av_strlcpy(s->filename, reply->location, sizeof(s->filename));
        rt->session_id[0] = '\0';
        av_log(s, AV_LOG_INFO, "Status %d: Redirecting to %s\n",
               reply->status_code,
               s->filename);
        goto redirect;
    }
    ff_network_close();
    return err;
}
