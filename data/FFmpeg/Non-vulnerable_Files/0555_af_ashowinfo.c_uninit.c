static av_cold void uninit(AVFilterContext *ctx)
{
    AShowInfoContext *s = ctx->priv;
    av_freep(&s->plane_checksums);
}
