static av_cold void uninit(AVFilterContext *ctx)
{
    ATADenoiseContext *s = ctx->priv;
    ff_bufqueue_discard_all(&s->q);
}
