static int open_stream(void *log, MovieStream *st)
{
    AVCodec *codec;
    int ret;
    codec = avcodec_find_decoder(st->st->codec->codec_id);
    if (!codec) {
        av_log(log, AV_LOG_ERROR, "Failed to find any codec\n");
        return AVERROR(EINVAL);
    }
    st->st->codec->refcounted_frames = 1;
    if ((ret = avcodec_open2(st->st->codec, codec, NULL)) < 0) {
        av_log(log, AV_LOG_ERROR, "Failed to open codec\n");
        return ret;
    }
    return 0;
}
