static int mpeg_decode_update_thread_context(AVCodecContext *avctx,
                                             const AVCodecContext *avctx_from)
{
    Mpeg1Context *ctx = avctx->priv_data, *ctx_from = avctx_from->priv_data;
    MpegEncContext *s = &ctx->mpeg_enc_ctx, *s1 = &ctx_from->mpeg_enc_ctx;
    int err;
    if (avctx == avctx_from               ||
        !ctx_from->mpeg_enc_ctx_allocated ||
        !s1->context_initialized)
        return 0;
    err = ff_mpeg_update_thread_context(avctx, avctx_from);
    if (err)
        return err;
    if (!ctx->mpeg_enc_ctx_allocated)
        memcpy(s + 1, s1 + 1, sizeof(Mpeg1Context) - sizeof(MpegEncContext));
    if (!(s->pict_type == AV_PICTURE_TYPE_B || s->low_delay))
        s->picture_number++;
    return 0;
}
