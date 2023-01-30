static av_cold void dejudder_uninit(AVFilterContext *ctx)
{
    DejudderContext *s = ctx->priv;
    av_freep(&(s->ringbuff));
}
