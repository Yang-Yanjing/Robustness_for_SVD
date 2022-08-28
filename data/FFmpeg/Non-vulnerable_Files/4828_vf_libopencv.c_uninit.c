static av_cold void uninit(AVFilterContext *ctx)
{
    OCVContext *s = ctx->priv;
    if (s->uninit)
        s->uninit(ctx);
    av_freep(&s->priv);
}
