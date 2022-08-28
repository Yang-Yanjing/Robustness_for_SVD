static void rtsp_parse_transport(RTSPMessageHeader *reply, const char *p)
{
    char transport_protocol[16];
    char profile[16];
    char lower_transport[16];
    char parameter[16];
    RTSPTransportField *th;
    char buf[256];
    reply->nb_transports = 0;
    for (;;) {
        p += strspn(p, SPACE_CHARS);
        if (*p == '\0')
            break;
        th = &reply->transports[reply->nb_transports];
        get_word_sep(transport_protocol, sizeof(transport_protocol),
                     "/", &p);
        if (!av_strcasecmp (transport_protocol, "rtp")) {
            get_word_sep(profile, sizeof(profile), "/;,", &p);
            lower_transport[0] = '\0';
            
            if (*p == '/') {
                get_word_sep(lower_transport, sizeof(lower_transport),
                             ";,", &p);
            }
            th->transport = RTSP_TRANSPORT_RTP;
        } else if (!av_strcasecmp (transport_protocol, "x-pn-tng") ||
                   !av_strcasecmp (transport_protocol, "x-real-rdt")) {
            
            get_word_sep(lower_transport, sizeof(lower_transport), "/;,", &p);
            profile[0] = '\0';
            th->transport = RTSP_TRANSPORT_RDT;
        } else if (!av_strcasecmp(transport_protocol, "raw")) {
            get_word_sep(profile, sizeof(profile), "/;,", &p);
            lower_transport[0] = '\0';
            
            if (*p == '/') {
                get_word_sep(lower_transport, sizeof(lower_transport),
                             ";,", &p);
            }
            th->transport = RTSP_TRANSPORT_RAW;
        }
        if (!av_strcasecmp(lower_transport, "TCP"))
            th->lower_transport = RTSP_LOWER_TRANSPORT_TCP;
        else
            th->lower_transport = RTSP_LOWER_TRANSPORT_UDP;
        if (*p == ';')
            p++;
        
        while (*p != '\0' && *p != ',') {
            get_word_sep(parameter, sizeof(parameter), "=;,", &p);
            if (!strcmp(parameter, "port")) {
                if (*p == '=') {
                    p++;
                    rtsp_parse_range(&th->port_min, &th->port_max, &p);
                }
            } else if (!strcmp(parameter, "client_port")) {
                if (*p == '=') {
                    p++;
                    rtsp_parse_range(&th->client_port_min,
                                     &th->client_port_max, &p);
                }
            } else if (!strcmp(parameter, "server_port")) {
                if (*p == '=') {
                    p++;
                    rtsp_parse_range(&th->server_port_min,
                                     &th->server_port_max, &p);
                }
            } else if (!strcmp(parameter, "interleaved")) {
                if (*p == '=') {
                    p++;
                    rtsp_parse_range(&th->interleaved_min,
                                     &th->interleaved_max, &p);
                }
            } else if (!strcmp(parameter, "multicast")) {
                if (th->lower_transport == RTSP_LOWER_TRANSPORT_UDP)
                    th->lower_transport = RTSP_LOWER_TRANSPORT_UDP_MULTICAST;
            } else if (!strcmp(parameter, "ttl")) {
                if (*p == '=') {
                    char *end;
                    p++;
                    th->ttl = strtol(p, &end, 10);
                    p = end;
                }
            } else if (!strcmp(parameter, "destination")) {
                if (*p == '=') {
                    p++;
                    get_word_sep(buf, sizeof(buf), ";,", &p);
                    get_sockaddr(buf, &th->destination);
                }
            } else if (!strcmp(parameter, "source")) {
                if (*p == '=') {
                    p++;
                    get_word_sep(buf, sizeof(buf), ";,", &p);
                    av_strlcpy(th->source, buf, sizeof(th->source));
                }
            } else if (!strcmp(parameter, "mode")) {
                if (*p == '=') {
                    p++;
                    get_word_sep(buf, sizeof(buf), ";, ", &p);
                    if (!strcmp(buf, "record") ||
                        !strcmp(buf, "receive"))
                        th->mode_record = 1;
                }
            }
            while (*p != ';' && *p != '\0' && *p != ',')
                p++;
            if (*p == ';')
                p++;
        }
        if (*p == ',')
            p++;
        reply->nb_transports++;
        if (reply->nb_transports >= RTSP_MAX_TRANSPORTS)
            break;
    }
}
