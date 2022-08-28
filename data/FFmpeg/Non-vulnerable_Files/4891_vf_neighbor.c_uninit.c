static av_cold void uninit(AVFilterContext *ctx)
{
    NContext *s = ctx->priv;
    av_freep(&s->buffer);
}
