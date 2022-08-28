static void finalize_rtp_handler_init(AVFormatContext *s, RTSPStream *rtsp_st,
                                      AVStream *st)
{
    if (rtsp_st->dynamic_handler && rtsp_st->dynamic_handler->init) {
        int ret = rtsp_st->dynamic_handler->init(s, st ? st->index : -1,
                                                 rtsp_st->dynamic_protocol_context);
        if (ret < 0) {
            if (rtsp_st->dynamic_protocol_context) {
                if (rtsp_st->dynamic_handler->close)
                    rtsp_st->dynamic_handler->close(
                        rtsp_st->dynamic_protocol_context);
                av_free(rtsp_st->dynamic_protocol_context);
            }
            rtsp_st->dynamic_protocol_context = NULL;
            rtsp_st->dynamic_handler = NULL;
        }
    }
}
