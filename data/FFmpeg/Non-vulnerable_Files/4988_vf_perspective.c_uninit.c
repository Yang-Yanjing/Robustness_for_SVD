static av_cold void uninit(AVFilterContext *ctx)
{
    PerspectiveContext *s = ctx->priv;
    av_freep(&s->pv);
}
