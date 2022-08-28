static av_cold void uninit(AVFilterContext *ctx)
{
    SineContext *sine = ctx->priv;
    av_freep(&sine->sin);
}
