static av_cold void uninit(AVFilterContext *ctx)
{
    GradFunContext *s = ctx->priv;
    av_freep(&s->buf);
}
