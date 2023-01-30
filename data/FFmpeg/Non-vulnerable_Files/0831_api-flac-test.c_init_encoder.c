static int init_encoder(AVCodec *enc, AVCodecContext **enc_ctx,
                        int64_t ch_layout, int sample_rate)
{
    AVCodecContext *ctx;
    int result;
    char name_buff[NAME_BUFF_SIZE];
    av_get_channel_layout_string(name_buff, NAME_BUFF_SIZE, 0, ch_layout);
    av_log(NULL, AV_LOG_INFO, "channel layout: %s, sample rate: %i\n", name_buff, sample_rate);
    ctx = avcodec_alloc_context3(enc);
    if (!ctx) {
        av_log(NULL, AV_LOG_ERROR, "Can't allocate encoder context\n");
        return AVERROR(ENOMEM);
    }
    ctx->sample_fmt = AV_SAMPLE_FMT_S16;
    ctx->sample_rate = sample_rate;
    ctx->channel_layout = ch_layout;
    result = avcodec_open2(ctx, enc, NULL);
    if (result < 0) {
        av_log(ctx, AV_LOG_ERROR, "Can't open encoder\n");
        return result;
    }
    *enc_ctx = ctx;
    return 0;
}
