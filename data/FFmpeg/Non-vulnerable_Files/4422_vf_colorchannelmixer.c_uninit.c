static av_cold void uninit(AVFilterContext *ctx)
{
    ColorChannelMixerContext *s = ctx->priv;
    av_freep(&s->buffer);
}
