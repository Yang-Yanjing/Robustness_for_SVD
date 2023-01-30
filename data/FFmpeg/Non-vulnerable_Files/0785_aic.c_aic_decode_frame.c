static int aic_decode_frame(AVCodecContext *avctx, void *data, int *got_frame,
                            AVPacket *avpkt)
{
    AICContext *ctx    = avctx->priv_data;
    const uint8_t *buf = avpkt->data;
    int buf_size       = avpkt->size;
    GetByteContext gb;
    uint32_t off;
    int x, y, ret;
    int slice_size;
    ctx->frame            = data;
    ctx->frame->pict_type = AV_PICTURE_TYPE_I;
    ctx->frame->key_frame = 1;
    off = FFALIGN(AIC_HDR_SIZE + ctx->num_x_slices * ctx->mb_height * 2, 4);
    if (buf_size < off) {
        av_log(avctx, AV_LOG_ERROR, "Too small frame\n");
        return AVERROR_INVALIDDATA;
    }
    ret = aic_decode_header(ctx, buf, buf_size);
    if (ret < 0) {
        av_log(avctx, AV_LOG_ERROR, "Invalid header\n");
        return ret;
    }
    if ((ret = ff_get_buffer(avctx, ctx->frame, 0)) < 0)
        return ret;
    bytestream2_init(&gb, buf + AIC_HDR_SIZE,
                     ctx->num_x_slices * ctx->mb_height * 2);
    for (y = 0; y < ctx->mb_height; y++) {
        for (x = 0; x < ctx->mb_width; x += ctx->slice_width) {
            slice_size = bytestream2_get_le16(&gb) * 4;
            if (slice_size + off > buf_size || !slice_size) {
                av_log(avctx, AV_LOG_ERROR,
                       "Incorrect slice size %d at %d.%d\n", slice_size, x, y);
                return AVERROR_INVALIDDATA;
            }
            ret = aic_decode_slice(ctx, x, y, buf + off, slice_size);
            if (ret < 0) {
                av_log(avctx, AV_LOG_ERROR,
                       "Error decoding slice at %d.%d\n", x, y);
                return ret;
            }
            off += slice_size;
        }
    }
    *got_frame = 1;
    return avpkt->size;
}
