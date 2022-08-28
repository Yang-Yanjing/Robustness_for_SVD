static void mpegts_find_stream_type(AVStream *st,
                                    uint32_t stream_type,
                                    const StreamType *types)
{
    if (avcodec_is_open(st->codec)) {
        av_log(NULL, AV_LOG_DEBUG, "cannot set stream info, codec is open\n");
        return;
    }
    for (; types->stream_type; types++)
        if (stream_type == types->stream_type) {
            st->codec->codec_type = types->codec_type;
            st->codec->codec_id   = types->codec_id;
            st->request_probe     = 0;
            return;
        }
}
