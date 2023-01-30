static av_cold int mpeg_decode_end(AVCodecContext *avctx)
{
    Mpeg1Context *s = avctx->priv_data;
    if (s->mpeg_enc_ctx_allocated)
        ff_mpv_common_end(&s->mpeg_enc_ctx);
    av_freep(&s->a53_caption);
    return 0;
}
