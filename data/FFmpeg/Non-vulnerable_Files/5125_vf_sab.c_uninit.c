static av_cold void uninit(AVFilterContext *ctx)
{
    SabContext *s = ctx->priv;
    close_filter_param(&s->luma);
    close_filter_param(&s->chroma);
}
