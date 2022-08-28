static av_cold void uninit(AVFilterContext *ctx)
{
    SilenceRemoveContext *s = ctx->priv;
    av_freep(&s->start_holdoff);
    av_freep(&s->stop_holdoff);
    av_freep(&s->window);
}
