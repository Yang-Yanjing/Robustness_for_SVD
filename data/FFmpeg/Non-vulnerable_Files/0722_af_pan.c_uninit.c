static av_cold void uninit(AVFilterContext *ctx)
{
    PanContext *pan = ctx->priv;
    swr_free(&pan->swr);
}
