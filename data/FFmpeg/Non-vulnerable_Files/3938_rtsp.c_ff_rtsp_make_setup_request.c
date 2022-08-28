int ff_rtsp_make_setup_request(AVFormatContext *s, const char *host, int port,
                              int lower_transport, const char *real_challenge)
{
    RTSPState *rt = s->priv_data;
    int rtx = 0, j, i, err, interleave = 0, port_off;
    RTSPStream *rtsp_st;
    RTSPMessageHeader reply1, *reply = &reply1;
    char cmd[2048];
    const char *trans_pref;
    if (rt->transport == RTSP_TRANSPORT_RDT)
        trans_pref = "x-pn-tng";
    else if (rt->transport == RTSP_TRANSPORT_RAW)
        trans_pref = "RAW/RAW";
    else
        trans_pref = "RTP/AVP";
    
    rt->timeout = 60;
    


    port_off = av_get_random_seed() % ((rt->rtp_port_max - rt->rtp_port_min)/2);
    
    port_off -= port_off & 0x01;
    for (j = rt->rtp_port_min + port_off, i = 0; i < rt->nb_rtsp_streams; ++i) {
        char transport[2048];
        




        if (lower_transport == RTSP_LOWER_TRANSPORT_UDP &&
             rt->server_type == RTSP_SERVER_WMS) {
            if (i == 0) {
                
                for (rtx = 0; rtx < rt->nb_rtsp_streams; rtx++) {
                    int len = strlen(rt->rtsp_streams[rtx]->control_url);
                    if (len >= 4 &&
                        !strcmp(rt->rtsp_streams[rtx]->control_url + len - 4,
                                "/rtx"))
                        break;
                }
                if (rtx == rt->nb_rtsp_streams)
                    return -1; 
                rtsp_st = rt->rtsp_streams[rtx];
            } else
                rtsp_st = rt->rtsp_streams[i > rtx ? i : i - 1];
        } else
            rtsp_st = rt->rtsp_streams[i];
        
        if (lower_transport == RTSP_LOWER_TRANSPORT_UDP) {
            char buf[256];
            if (rt->server_type == RTSP_SERVER_WMS && i > 1) {
                port = reply->transports[0].client_port_min;
                goto have_port;
            }
            
            while (j <= rt->rtp_port_max) {
                AVDictionary *opts = map_to_opts(rt);
                ff_url_join(buf, sizeof(buf), "rtp", NULL, host, -1,
                            "?localport=%d", j);
                
                j += 2;
                err = ffurl_open(&rtsp_st->rtp_handle, buf, AVIO_FLAG_READ_WRITE,
                                 &s->interrupt_callback, &opts);
                av_dict_free(&opts);
                if (!err)
                    goto rtp_opened;
            }
            av_log(s, AV_LOG_ERROR, "Unable to open an input RTP port\n");
            err = AVERROR(EIO);
            goto fail;
        rtp_opened:
            port = ff_rtp_get_local_rtp_port(rtsp_st->rtp_handle);
        have_port:
            snprintf(transport, sizeof(transport) - 1,
                     "%s/UDP;", trans_pref);
            if (rt->server_type != RTSP_SERVER_REAL)
                av_strlcat(transport, "unicast;", sizeof(transport));
            av_strlcatf(transport, sizeof(transport),
                     "client_port=%d", port);
            if (rt->transport == RTSP_TRANSPORT_RTP &&
                !(rt->server_type == RTSP_SERVER_WMS && i > 0))
                av_strlcatf(transport, sizeof(transport), "-%d", port + 1);
        }
        
        else if (lower_transport == RTSP_LOWER_TRANSPORT_TCP) {
            


            if (rt->server_type == RTSP_SERVER_WMS &&
                (rtsp_st->stream_index < 0 ||
                 s->streams[rtsp_st->stream_index]->codec->codec_type ==
                    AVMEDIA_TYPE_DATA))
                continue;
            snprintf(transport, sizeof(transport) - 1,
                     "%s/TCP;", trans_pref);
            if (rt->transport != RTSP_TRANSPORT_RDT)
                av_strlcat(transport, "unicast;", sizeof(transport));
            av_strlcatf(transport, sizeof(transport),
                        "interleaved=%d-%d",
                        interleave, interleave + 1);
            interleave += 2;
        }
        else if (lower_transport == RTSP_LOWER_TRANSPORT_UDP_MULTICAST) {
            snprintf(transport, sizeof(transport) - 1,
                     "%s/UDP;multicast", trans_pref);
        }
        if (s->oformat) {
            av_strlcat(transport, ";mode=record", sizeof(transport));
        } else if (rt->server_type == RTSP_SERVER_REAL ||
                   rt->server_type == RTSP_SERVER_WMS)
            av_strlcat(transport, ";mode=play", sizeof(transport));
        snprintf(cmd, sizeof(cmd),
                 "Transport: %s\r\n",
                 transport);
        if (rt->accept_dynamic_rate)
            av_strlcat(cmd, "x-Dynamic-Rate: 0\r\n", sizeof(cmd));
        if (CONFIG_RTPDEC && i == 0 && rt->server_type == RTSP_SERVER_REAL) {
            char real_res[41], real_csum[9];
            ff_rdt_calc_response_and_checksum(real_res, real_csum,
                                              real_challenge);
            av_strlcatf(cmd, sizeof(cmd),
                        "If-Match: %s\r\n"
                        "RealChallenge2: %s, sd=%s\r\n",
                        rt->session_id, real_res, real_csum);
        }
        ff_rtsp_send_cmd(s, "SETUP", rtsp_st->control_url, cmd, reply, NULL);
        if (reply->status_code == 461  && i == 0) {
            err = 1;
            goto fail;
        } else if (reply->status_code != RTSP_STATUS_OK ||
                   reply->nb_transports != 1) {
            err = ff_rtsp_averror(reply->status_code, AVERROR_INVALIDDATA);
            goto fail;
        }
        
        if (i > 0) {
            if (reply->transports[0].lower_transport != rt->lower_transport ||
                reply->transports[0].transport != rt->transport) {
                err = AVERROR_INVALIDDATA;
                goto fail;
            }
        } else {
            rt->lower_transport = reply->transports[0].lower_transport;
            rt->transport = reply->transports[0].transport;
        }
        

        if (reply->transports[0].lower_transport != lower_transport) {
            av_log(s, AV_LOG_ERROR, "Nonmatching transport in server reply\n");
            err = AVERROR_INVALIDDATA;
            goto fail;
        }
        switch(reply->transports[0].lower_transport) {
        case RTSP_LOWER_TRANSPORT_TCP:
            rtsp_st->interleaved_min = reply->transports[0].interleaved_min;
            rtsp_st->interleaved_max = reply->transports[0].interleaved_max;
            break;
        case RTSP_LOWER_TRANSPORT_UDP: {
            char url[1024], options[30] = "";
            const char *peer = host;
            if (rt->rtsp_flags & RTSP_FLAG_FILTER_SRC)
                av_strlcpy(options, "?connect=1", sizeof(options));
            
            if (reply->transports[0].source[0])
                peer = reply->transports[0].source;
            ff_url_join(url, sizeof(url), "rtp", NULL, peer,
                        reply->transports[0].server_port_min, "%s", options);
            if (!(rt->server_type == RTSP_SERVER_WMS && i > 1) &&
                ff_rtp_set_remote_url(rtsp_st->rtp_handle, url) < 0) {
                err = AVERROR_INVALIDDATA;
                goto fail;
            }
            break;
        }
        case RTSP_LOWER_TRANSPORT_UDP_MULTICAST: {
            char url[1024], namebuf[50], optbuf[20] = "";
            struct sockaddr_storage addr;
            int port, ttl;
            if (reply->transports[0].destination.ss_family) {
                addr      = reply->transports[0].destination;
                port      = reply->transports[0].port_min;
                ttl       = reply->transports[0].ttl;
            } else {
                addr      = rtsp_st->sdp_ip;
                port      = rtsp_st->sdp_port;
                ttl       = rtsp_st->sdp_ttl;
            }
            if (ttl > 0)
                snprintf(optbuf, sizeof(optbuf), "?ttl=%d", ttl);
            getnameinfo((struct sockaddr*) &addr, sizeof(addr),
                        namebuf, sizeof(namebuf), NULL, 0, NI_NUMERICHOST);
            ff_url_join(url, sizeof(url), "rtp", NULL, namebuf,
                        port, "%s", optbuf);
            if (ffurl_open(&rtsp_st->rtp_handle, url, AVIO_FLAG_READ_WRITE,
                           &s->interrupt_callback, NULL) < 0) {
                err = AVERROR_INVALIDDATA;
                goto fail;
            }
            break;
        }
        }
        if ((err = ff_rtsp_open_transport_ctx(s, rtsp_st)))
            goto fail;
    }
    if (rt->nb_rtsp_streams && reply->timeout > 0)
        rt->timeout = reply->timeout;
    if (rt->server_type == RTSP_SERVER_REAL)
        rt->need_subscription = 1;
    return 0;
fail:
    ff_rtsp_undo_setup(s, 0);
    return err;
}
