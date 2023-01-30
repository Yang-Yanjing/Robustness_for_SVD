static av_cold void uninit(AVFilterContext *ctx)
{
    SPPContext *s = ctx->priv;
    av_freep(&s->temp);
    av_freep(&s->src);
    if (s->avctx) {
        avcodec_close(s->avctx);
        av_freep(&s->avctx);
    }
    av_freep(&s->dct);
    av_freep(&s->non_b_qp_table);
}
