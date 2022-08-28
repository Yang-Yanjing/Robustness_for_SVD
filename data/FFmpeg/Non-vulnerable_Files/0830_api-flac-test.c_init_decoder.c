static int init_decoder(AVCodec *dec, AVCodecContext **dec_ctx,
                        int64_t ch_layout)
{
    AVCodecContext *ctx;
    int result;
    ctx = avcodec_alloc_context3(dec);
    if (!ctx) {
        av_log(NULL, AV_LOG_ERROR , "Can't allocate decoder context\n");
        return AVERROR(ENOMEM);
    }
    ctx->request_sample_fmt = AV_SAMPLE_FMT_S16;
    
    ctx->request_channel_layout = ch_layout;
    ctx->channel_layout = ch_layout;
    result = avcodec_open2(ctx, dec, NULL);
    if (result < 0) {
        av_log(ctx, AV_LOG_ERROR, "Can't open decoder\n");
        return result;
    }
    *dec_ctx = ctx;
    return 0;
}
