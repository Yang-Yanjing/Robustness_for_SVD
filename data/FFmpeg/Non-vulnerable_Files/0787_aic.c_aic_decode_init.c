static av_cold int aic_decode_init(AVCodecContext *avctx)
{
    AICContext *ctx = avctx->priv_data;
    int i;
    uint8_t scan[64];
    ctx->avctx = avctx;
    avctx->pix_fmt = AV_PIX_FMT_YUV420P;
    ff_idctdsp_init(&ctx->idsp, avctx);
    for (i = 0; i < 64; i++)
        scan[i] = i;
    ff_init_scantable(ctx->idsp.idct_permutation, &ctx->scantable, scan);
    for (i = 0; i < 64; i++)
        ctx->quant_matrix[ctx->idsp.idct_permutation[i]] = aic_quant_matrix[i];
    ctx->mb_width  = FFALIGN(avctx->width,  16) >> 4;
    ctx->mb_height = FFALIGN(avctx->height, 16) >> 4;
    ctx->num_x_slices = (ctx->mb_width + 15) >> 4;
    ctx->slice_width  = 16;
    for (i = 1; i < 32; i++) {
        if (!(ctx->mb_width % i) && (ctx->mb_width / i <= 32)) {
            ctx->slice_width  = ctx->mb_width / i;
            ctx->num_x_slices = i;
            break;
        }
    }
    ctx->slice_data = av_malloc_array(ctx->slice_width, AIC_BAND_COEFFS
                                * sizeof(*ctx->slice_data));
    if (!ctx->slice_data) {
        av_log(avctx, AV_LOG_ERROR, "Error allocating slice buffer\n");
        return AVERROR(ENOMEM);
    }
    for (i = 0; i < NUM_BANDS; i++)
        ctx->data_ptr[i] = ctx->slice_data + ctx->slice_width
                                             * aic_band_off[i];
    return 0;
}
