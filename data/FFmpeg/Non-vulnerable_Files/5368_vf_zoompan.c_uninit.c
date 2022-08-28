static av_cold void uninit(AVFilterContext *ctx)
{
    ZPContext *s = ctx->priv;
    sws_freeContext(s->sws);
    s->sws = NULL;
}
