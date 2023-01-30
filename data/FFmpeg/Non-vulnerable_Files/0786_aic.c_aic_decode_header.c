static int aic_decode_header(AICContext *ctx, const uint8_t *src, int size)
{
    uint32_t frame_size;
    int width, height;
    if (src[0] != 1) {
        av_log(ctx->avctx, AV_LOG_ERROR, "Invalid version %d\n", src[0]);
        return AVERROR_INVALIDDATA;
    }
    if (src[1] != AIC_HDR_SIZE - 2) {
        av_log(ctx->avctx, AV_LOG_ERROR, "Invalid header size %d\n", src[1]);
        return AVERROR_INVALIDDATA;
    }
    frame_size = AV_RB32(src + 2);
    width      = AV_RB16(src + 6);
    height     = AV_RB16(src + 8);
    if (frame_size > size) {
        av_log(ctx->avctx, AV_LOG_ERROR, "Frame size should be %"PRIu32" got %d\n",
               frame_size, size);
        return AVERROR_INVALIDDATA;
    }
    if (width != ctx->avctx->width || height != ctx->avctx->height) {
        av_log(ctx->avctx, AV_LOG_ERROR,
               "Picture dimension changed: old: %d x %d, new: %d x %d\n",
               ctx->avctx->width, ctx->avctx->height, width, height);
        return AVERROR_INVALIDDATA;
    }
    ctx->quant      = src[15];
    ctx->interlaced = ((src[16] >> 4) == 3);
    return 0;
}
