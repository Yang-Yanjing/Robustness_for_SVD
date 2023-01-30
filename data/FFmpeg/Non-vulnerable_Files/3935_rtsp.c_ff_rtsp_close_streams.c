void ff_rtsp_close_streams(AVFormatContext *s)
{
    RTSPState *rt = s->priv_data;
    int i, j;
    RTSPStream *rtsp_st;
    ff_rtsp_undo_setup(s, 0);
    for (i = 0; i < rt->nb_rtsp_streams; i++) {
        rtsp_st = rt->rtsp_streams[i];
        if (rtsp_st) {
            if (rtsp_st->dynamic_handler && rtsp_st->dynamic_protocol_context) {
                if (rtsp_st->dynamic_handler->close)
                    rtsp_st->dynamic_handler->close(
                        rtsp_st->dynamic_protocol_context);
                av_free(rtsp_st->dynamic_protocol_context);
            }
            for (j = 0; j < rtsp_st->nb_include_source_addrs; j++)
                av_freep(&rtsp_st->include_source_addrs[j]);
            av_freep(&rtsp_st->include_source_addrs);
            for (j = 0; j < rtsp_st->nb_exclude_source_addrs; j++)
                av_freep(&rtsp_st->exclude_source_addrs[j]);
            av_freep(&rtsp_st->exclude_source_addrs);
            av_freep(&rtsp_st);
        }
    }
    av_freep(&rt->rtsp_streams);
    if (rt->asf_ctx) {
        avformat_close_input(&rt->asf_ctx);
    }
    if (CONFIG_RTPDEC && rt->ts)
        avpriv_mpegts_parse_close(rt->ts);
    av_freep(&rt->p);
    av_freep(&rt->recvbuf);
}
