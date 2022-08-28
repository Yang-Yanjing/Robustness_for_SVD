int ff_rtsp_read_reply(AVFormatContext *s, RTSPMessageHeader *reply,
                       unsigned char **content_ptr,
                       int return_on_interleaved_data, const char *method)
{
    RTSPState *rt = s->priv_data;
    char buf[4096], buf1[1024], *q;
    unsigned char ch;
    const char *p;
    int ret, content_length, line_count = 0, request = 0;
    int first_line = 1;
    unsigned char *content = NULL;
start:
    line_count = 0;
    request = 0;
    content = NULL;
    memset(reply, 0, sizeof(*reply));
    
    rt->last_reply[0] = '\0';
    for (;;) {
        q = buf;
        for (;;) {
            ret = ffurl_read_complete(rt->rtsp_hd, &ch, 1);
            av_log(s, AV_LOG_TRACE, "ret=%d c=%02x [%c]\n", ret, ch, ch);
            if (ret != 1)
                return AVERROR_EOF;
            if (ch == '\n')
                break;
            if (ch == '$' && first_line && q == buf) {
                if (return_on_interleaved_data) {
                    return 1;
                } else
                    ff_rtsp_skip_packet(s);
            } else if (ch != '\r') {
                if ((q - buf) < sizeof(buf) - 1)
                    *q++ = ch;
            }
        }
        *q = '\0';
        first_line = 0;
        av_log(s, AV_LOG_TRACE, "line='%s'\n", buf);
        
        if (buf[0] == '\0')
            break;
        p = buf;
        if (line_count == 0) {
            
            get_word(buf1, sizeof(buf1), &p);
            if (!strncmp(buf1, "RTSP/", 5)) {
                get_word(buf1, sizeof(buf1), &p);
                reply->status_code = atoi(buf1);
                av_strlcpy(reply->reason, p, sizeof(reply->reason));
            } else {
                av_strlcpy(reply->reason, buf1, sizeof(reply->reason)); 
                get_word(buf1, sizeof(buf1), &p); 
                request = 1;
            }
        } else {
            ff_rtsp_parse_line(reply, p, rt, method);
            av_strlcat(rt->last_reply, p,    sizeof(rt->last_reply));
            av_strlcat(rt->last_reply, "\n", sizeof(rt->last_reply));
        }
        line_count++;
    }
    if (rt->session_id[0] == '\0' && reply->session_id[0] != '\0' && !request)
        av_strlcpy(rt->session_id, reply->session_id, sizeof(rt->session_id));
    content_length = reply->content_length;
    if (content_length > 0) {
        
        content = av_malloc(content_length + 1);
        if (!content)
            return AVERROR(ENOMEM);
        ffurl_read_complete(rt->rtsp_hd, content, content_length);
        content[content_length] = '\0';
    }
    if (content_ptr)
        *content_ptr = content;
    else
        av_freep(&content);
    if (request) {
        char buf[1024];
        char base64buf[AV_BASE64_SIZE(sizeof(buf))];
        const char* ptr = buf;
        if (!strcmp(reply->reason, "OPTIONS")) {
            snprintf(buf, sizeof(buf), "RTSP/1.0 200 OK\r\n");
            if (reply->seq)
                av_strlcatf(buf, sizeof(buf), "CSeq: %d\r\n", reply->seq);
            if (reply->session_id[0])
                av_strlcatf(buf, sizeof(buf), "Session: %s\r\n",
                                              reply->session_id);
        } else {
            snprintf(buf, sizeof(buf), "RTSP/1.0 501 Not Implemented\r\n");
        }
        av_strlcat(buf, "\r\n", sizeof(buf));
        if (rt->control_transport == RTSP_MODE_TUNNEL) {
            av_base64_encode(base64buf, sizeof(base64buf), buf, strlen(buf));
            ptr = base64buf;
        }
        ffurl_write(rt->rtsp_hd_out, ptr, strlen(ptr));
        rt->last_cmd_time = av_gettime_relative();
        


        if (content_ptr)
            av_freep(content_ptr);
        



        if (method)
            goto start;
        return 0;
    }
    if (rt->seq != reply->seq) {
        av_log(s, AV_LOG_WARNING, "CSeq %d expected, %d received.\n",
            rt->seq, reply->seq);
    }
    
    if (reply->notice == 2101       ||
        reply->notice == 2104     ||
        reply->notice == 2306 ) {
        rt->state = RTSP_STATE_IDLE;
    } else if (reply->notice >= 4400 && reply->notice < 5500) {
        return AVERROR(EIO); 
    } else if (reply->notice == 2401  ||
             (reply->notice >= 5500 && reply->notice < 5600)  )
        return AVERROR(EPERM);
    return 0;
}
