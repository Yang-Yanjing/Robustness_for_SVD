AVFILTER_DEFINE_CLASS(mcdeint);
static int config_props(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    MCDeintContext *mcdeint = ctx->priv;
    AVCodec *enc;
    AVCodecContext *enc_ctx;
    AVDictionary *opts = NULL;
    int ret;
    if (!(enc = avcodec_find_encoder(AV_CODEC_ID_SNOW))) {
        av_log(ctx, AV_LOG_ERROR, "Snow encoder is not enabled in libavcodec\n");
        return AVERROR(EINVAL);
    }
    mcdeint->enc_ctx = avcodec_alloc_context3(enc);
    if (!mcdeint->enc_ctx)
        return AVERROR(ENOMEM);
    enc_ctx = mcdeint->enc_ctx;
    enc_ctx->width  = inlink->w;
    enc_ctx->height = inlink->h;
    enc_ctx->time_base = (AVRational){1,25};  
    enc_ctx->gop_size = INT_MAX;
    enc_ctx->max_b_frames = 0;
    enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    enc_ctx->flags = AV_CODEC_FLAG_QSCALE | CODEC_FLAG_LOW_DELAY;
    enc_ctx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    enc_ctx->global_quality = 1;
    enc_ctx->me_cmp = enc_ctx->me_sub_cmp = FF_CMP_SAD;
    enc_ctx->mb_cmp = FF_CMP_SSE;
    av_dict_set(&opts, "memc_only", "1", 0);
    av_dict_set(&opts, "no_bitstream", "1", 0);
    switch (mcdeint->mode) {
    case MODE_EXTRA_SLOW:
        enc_ctx->refs = 3;
    case MODE_SLOW:
        enc_ctx->me_method = ME_ITER;
    case MODE_MEDIUM:
        enc_ctx->flags |= AV_CODEC_FLAG_4MV;
        enc_ctx->dia_size = 2;
    case MODE_FAST:
        enc_ctx->flags |= AV_CODEC_FLAG_QPEL;
    }
    ret = avcodec_open2(enc_ctx, enc, &opts);
    av_dict_free(&opts);
    if (ret < 0)
        return ret;
    return 0;
}
