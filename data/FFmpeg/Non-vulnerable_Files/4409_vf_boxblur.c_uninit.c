static av_cold void uninit(AVFilterContext *ctx)
{
    BoxBlurContext *s = ctx->priv;
    av_freep(&s->temp[0]);
    av_freep(&s->temp[1]);
}
