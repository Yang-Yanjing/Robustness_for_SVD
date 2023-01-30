static av_cold void uninit(AVFilterContext *ctx)
{
    FormatContext *s = ctx->priv;
    av_freep(&s->formats);
}
