static av_cold void uninit(AVFilterContext *ctx)
{
    ReverseContext *s = ctx->priv;
    av_freep(&s->pts);
    av_freep(&s->frames);
}
