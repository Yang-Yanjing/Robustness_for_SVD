void ff_rtsp_parse_line(RTSPMessageHeader *reply, const char *buf,
                        RTSPState *rt, const char *method)
{
    const char *p;
    
    p = buf;
    if (av_stristart(p, "Session:", &p)) {
        int t;
        get_word_sep(reply->session_id, sizeof(reply->session_id), ";", &p);
        if (av_stristart(p, ";timeout=", &p) &&
            (t = strtol(p, NULL, 10)) > 0) {
            reply->timeout = t;
        }
    } else if (av_stristart(p, "Content-Length:", &p)) {
        reply->content_length = strtol(p, NULL, 10);
    } else if (av_stristart(p, "Transport:", &p)) {
        rtsp_parse_transport(reply, p);
    } else if (av_stristart(p, "CSeq:", &p)) {
        reply->seq = strtol(p, NULL, 10);
    } else if (av_stristart(p, "Range:", &p)) {
        rtsp_parse_range_npt(p, &reply->range_start, &reply->range_end);
    } else if (av_stristart(p, "RealChallenge1:", &p)) {
        p += strspn(p, SPACE_CHARS);
        av_strlcpy(reply->real_challenge, p, sizeof(reply->real_challenge));
    } else if (av_stristart(p, "Server:", &p)) {
        p += strspn(p, SPACE_CHARS);
        av_strlcpy(reply->server, p, sizeof(reply->server));
    } else if (av_stristart(p, "Notice:", &p) ||
               av_stristart(p, "X-Notice:", &p)) {
        reply->notice = strtol(p, NULL, 10);
    } else if (av_stristart(p, "Location:", &p)) {
        p += strspn(p, SPACE_CHARS);
        av_strlcpy(reply->location, p , sizeof(reply->location));
    } else if (av_stristart(p, "WWW-Authenticate:", &p) && rt) {
        p += strspn(p, SPACE_CHARS);
        ff_http_auth_handle_header(&rt->auth_state, "WWW-Authenticate", p);
    } else if (av_stristart(p, "Authentication-Info:", &p) && rt) {
        p += strspn(p, SPACE_CHARS);
        ff_http_auth_handle_header(&rt->auth_state, "Authentication-Info", p);
    } else if (av_stristart(p, "Content-Base:", &p) && rt) {
        p += strspn(p, SPACE_CHARS);
        if (method && !strcmp(method, "DESCRIBE"))
            av_strlcpy(rt->control_uri, p , sizeof(rt->control_uri));
    } else if (av_stristart(p, "RTP-Info:", &p) && rt) {
        p += strspn(p, SPACE_CHARS);
        if (method && !strcmp(method, "PLAY"))
            rtsp_parse_rtp_info(rt, p);
    } else if (av_stristart(p, "Public:", &p) && rt) {
        if (strstr(p, "GET_PARAMETER") &&
            method && !strcmp(method, "OPTIONS"))
            rt->get_parameter_supported = 1;
    } else if (av_stristart(p, "x-Accept-Dynamic-Rate:", &p) && rt) {
        p += strspn(p, SPACE_CHARS);
        rt->accept_dynamic_rate = atoi(p);
    } else if (av_stristart(p, "Content-Type:", &p)) {
        p += strspn(p, SPACE_CHARS);
        av_strlcpy(reply->content_type, p, sizeof(reply->content_type));
    }
}
