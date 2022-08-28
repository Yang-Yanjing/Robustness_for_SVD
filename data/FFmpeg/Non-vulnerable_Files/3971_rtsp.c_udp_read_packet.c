static int udp_read_packet(AVFormatContext *s, RTSPStream **prtsp_st,
                           uint8_t *buf, int buf_size, int64_t wait_end)
{
    RTSPState *rt = s->priv_data;
    RTSPStream *rtsp_st;
    int n, i, ret, tcp_fd, timeout_cnt = 0;
    int max_p = 0;
    struct pollfd *p = rt->p;
    int *fds = NULL, fdsnum, fdsidx;
    for (;;) {
        if (ff_check_interrupt(&s->interrupt_callback))
            return AVERROR_EXIT;
        if (wait_end && wait_end - av_gettime_relative() < 0)
            return AVERROR(EAGAIN);
        max_p = 0;
        if (rt->rtsp_hd) {
            tcp_fd = ffurl_get_file_handle(rt->rtsp_hd);
            p[max_p].fd = tcp_fd;
            p[max_p++].events = POLLIN;
        } else {
            tcp_fd = -1;
        }
        for (i = 0; i < rt->nb_rtsp_streams; i++) {
            rtsp_st = rt->rtsp_streams[i];
            if (rtsp_st->rtp_handle) {
                if (ret = ffurl_get_multi_file_handle(rtsp_st->rtp_handle,
                                                      &fds, &fdsnum)) {
                    av_log(s, AV_LOG_ERROR, "Unable to recover rtp ports\n");
                    return ret;
                }
                if (fdsnum != 2) {
                    av_log(s, AV_LOG_ERROR,
                           "Number of fds %d not supported\n", fdsnum);
                    return AVERROR_INVALIDDATA;
                }
                for (fdsidx = 0; fdsidx < fdsnum; fdsidx++) {
                    p[max_p].fd       = fds[fdsidx];
                    p[max_p++].events = POLLIN;
                }
                av_freep(&fds);
            }
        }
        n = poll(p, max_p, POLL_TIMEOUT_MS);
        if (n > 0) {
            int j = 1 - (tcp_fd == -1);
            timeout_cnt = 0;
            for (i = 0; i < rt->nb_rtsp_streams; i++) {
                rtsp_st = rt->rtsp_streams[i];
                if (rtsp_st->rtp_handle) {
                    if (p[j].revents & POLLIN || p[j+1].revents & POLLIN) {
                        ret = ffurl_read(rtsp_st->rtp_handle, buf, buf_size);
                        if (ret > 0) {
                            *prtsp_st = rtsp_st;
                            return ret;
                        }
                    }
                    j+=2;
                }
            }
#if CONFIG_RTSP_DEMUXER
            if (tcp_fd != -1 && p[0].revents & POLLIN) {
                if (rt->rtsp_flags & RTSP_FLAG_LISTEN) {
                    if (rt->state == RTSP_STATE_STREAMING) {
                        if (!ff_rtsp_parse_streaming_commands(s))
                            return AVERROR_EOF;
                        else
                            av_log(s, AV_LOG_WARNING,
                                   "Unable to answer to TEARDOWN\n");
                    } else
                        return 0;
                } else {
                    RTSPMessageHeader reply;
                    ret = ff_rtsp_read_reply(s, &reply, NULL, 0, NULL);
                    if (ret < 0)
                        return ret;
                    
                    if (rt->state != RTSP_STATE_STREAMING)
                        return 0;
                }
            }
#endif
        } else if (n == 0 && ++timeout_cnt >= MAX_TIMEOUTS) {
            return AVERROR(ETIMEDOUT);
        } else if (n < 0 && errno != EINTR)
            return AVERROR(errno);
    }
}
