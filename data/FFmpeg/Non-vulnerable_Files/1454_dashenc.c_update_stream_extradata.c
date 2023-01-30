static int update_stream_extradata(AVFormatContext *s, OutputStream *os,
                                   AVCodecContext *codec)
{
    uint8_t *extradata;
    if (os->ctx->streams[0]->codec->extradata_size || !codec->extradata_size)
        return 0;
    extradata = av_malloc(codec->extradata_size);
    if (!extradata)
        return AVERROR(ENOMEM);
    memcpy(extradata, codec->extradata, codec->extradata_size);
    os->ctx->streams[0]->codec->extradata = extradata;
    os->ctx->streams[0]->codec->extradata_size = codec->extradata_size;
    set_codec_str(s, codec, os->codec_str, sizeof(os->codec_str));
    return 0;
}
