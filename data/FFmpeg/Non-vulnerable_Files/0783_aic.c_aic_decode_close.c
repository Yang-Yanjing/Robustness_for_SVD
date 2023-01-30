static av_cold int aic_decode_close(AVCodecContext *avctx)
{
    AICContext *ctx = avctx->priv_data;
    av_freep(&ctx->slice_data);
    return 0;
}
