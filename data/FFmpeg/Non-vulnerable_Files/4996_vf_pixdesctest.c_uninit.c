static av_cold void uninit(AVFilterContext *ctx)
{
    PixdescTestContext *priv = ctx->priv;
    av_freep(&priv->line);
}
