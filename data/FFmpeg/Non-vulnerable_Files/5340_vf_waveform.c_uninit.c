static av_cold void uninit(AVFilterContext *ctx)
{
    WaveformContext *s = ctx->priv;
    av_freep(&s->peak);
}
