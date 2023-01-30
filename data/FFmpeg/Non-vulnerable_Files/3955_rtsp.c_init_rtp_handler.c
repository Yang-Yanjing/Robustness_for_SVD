static void init_rtp_handler(RTPDynamicProtocolHandler *handler,
                             RTSPStream *rtsp_st, AVStream *st)
{
    AVCodecContext *codec = st ? st->codec : NULL;
    if (!handler)
        return;
    if (codec)
        codec->codec_id          = handler->codec_id;
    rtsp_st->dynamic_handler = handler;
    if (st)
        st->need_parsing = handler->need_parsing;
    if (handler->priv_data_size) {
        rtsp_st->dynamic_protocol_context = av_mallocz(handler->priv_data_size);
        if (!rtsp_st->dynamic_protocol_context)
            rtsp_st->dynamic_handler = NULL;
    }
}
