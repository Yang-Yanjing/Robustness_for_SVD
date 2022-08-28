static int rtsp_send_cmd_with_content_async(AVFormatContext *s,
                                            const char *method, const char *url,
                                            const char *headers,
                                            const unsigned char *send_content,
                                            int send_content_length)
{
    RTSPState *rt = s->priv_data;
    char buf[4096], *out_buf;
    char base64buf[AV_BASE64_SIZE(sizeof(buf))];
    
    out_buf = buf;
    rt->seq++;
    snprintf(buf, sizeof(buf), "%s %s RTSP/1.0\r\n", method, url);
    if (headers)
        av_strlcat(buf, headers, sizeof(buf));
    av_strlcatf(buf, sizeof(buf), "CSeq: %d\r\n", rt->seq);
    av_strlcatf(buf, sizeof(buf), "User-Agent: %s\r\n",  rt->user_agent);
    if (rt->session_id[0] != '\0' && (!headers ||
        !strstr(headers, "\nIf-Match:"))) {
        av_strlcatf(buf, sizeof(buf), "Session: %s\r\n", rt->session_id);
    }
    if (rt->auth[0]) {
        char *str = ff_http_auth_create_response(&rt->auth_state,
                                                 rt->auth, url, method);
        if (str)
            av_strlcat(buf, str, sizeof(buf));
        av_free(str);
    }
    if (send_content_length > 0 && send_content)
        av_strlcatf(buf, sizeof(buf), "Content-Length: %d\r\n", send_content_length);
    av_strlcat(buf, "\r\n", sizeof(buf));
    
    if (rt->control_transport == RTSP_MODE_TUNNEL) {
        av_base64_encode(base64buf, sizeof(base64buf), buf, strlen(buf));
        out_buf = base64buf;
    }
    av_log(s, AV_LOG_TRACE, "Sending:\n%s--\n", buf);
    ffurl_write(rt->rtsp_hd_out, out_buf, strlen(out_buf));
    if (send_content_length > 0 && send_content) {
        if (rt->control_transport == RTSP_MODE_TUNNEL) {
            av_log(s, AV_LOG_ERROR, "tunneling of RTSP requests "
                                    "with content data not supported\n");
            return AVERROR_PATCHWELCOME;
        }
        ffurl_write(rt->rtsp_hd_out, send_content, send_content_length);
    }
    rt->last_cmd_time = av_gettime_relative();
    return 0;
}
