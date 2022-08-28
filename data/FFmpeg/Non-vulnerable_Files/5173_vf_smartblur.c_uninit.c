static av_cold void uninit(AVFilterContext *ctx)
{
    SmartblurContext *s = ctx->priv;
    sws_freeContext(s->luma.filter_context);
    sws_freeContext(s->chroma.filter_context);
}
