static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    USPPContext *uspp = ctx->priv;
    const int height = inlink->h;
    const int width  = inlink->w;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int i;
    AVCodec *enc = avcodec_find_encoder(AV_CODEC_ID_SNOW);
    if (!enc) {
        av_log(ctx, AV_LOG_ERROR, "SNOW encoder not found.\n");
        return AVERROR(EINVAL);
    }
    uspp->hsub = desc->log2_chroma_w;
    uspp->vsub = desc->log2_chroma_h;
    for (i = 0; i < 3; i++) {
        int is_chroma = !!i;
        int w = (width  + 4 * BLOCK-1) & (~(2 * BLOCK-1));
        int h = (height + 4 * BLOCK-1) & (~(2 * BLOCK-1));
        if (is_chroma) {
            w = FF_CEIL_RSHIFT(w, uspp->hsub);
            h = FF_CEIL_RSHIFT(h, uspp->vsub);
        }
        uspp->temp_stride[i] = w;
        if (!(uspp->temp[i] = av_malloc_array(uspp->temp_stride[i], h * sizeof(int16_t))))
            return AVERROR(ENOMEM);
        if (!(uspp->src [i] = av_malloc_array(uspp->temp_stride[i], h * sizeof(uint8_t))))
            return AVERROR(ENOMEM);
    }
    for (i = 0; i < (1<<uspp->log2_count); i++) {
        AVCodecContext *avctx_enc;
        AVDictionary *opts = NULL;
        int ret;
        if (!(uspp->avctx_enc[i] = avcodec_alloc_context3(NULL)))
            return AVERROR(ENOMEM);
        avctx_enc = uspp->avctx_enc[i];
        avctx_enc->width = width + BLOCK;
        avctx_enc->height = height + BLOCK;
        avctx_enc->time_base = (AVRational){1,25};  
        avctx_enc->gop_size = INT_MAX;
        avctx_enc->max_b_frames = 0;
        avctx_enc->pix_fmt = inlink->format;
        avctx_enc->flags = AV_CODEC_FLAG_QSCALE | CODEC_FLAG_LOW_DELAY;
        avctx_enc->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
        avctx_enc->global_quality = 123;
        av_dict_set(&opts, "no_bitstream", "1", 0);
        ret = avcodec_open2(avctx_enc, enc, &opts);
        if (ret < 0)
            return ret;
        av_dict_free(&opts);
        av_assert0(avctx_enc->codec);
    }
    uspp->outbuf_size = (width + BLOCK) * (height + BLOCK) * 10;
    if (!(uspp->frame = av_frame_alloc()))
        return AVERROR(ENOMEM);
    if (!(uspp->outbuf = av_malloc(uspp->outbuf_size)))
        return AVERROR(ENOMEM);
    return 0;
}
