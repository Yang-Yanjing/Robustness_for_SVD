static av_cold void uninit(AVFilterContext *ctx)
{
    DecimateContext *decimate = ctx->priv;
    av_frame_free(&decimate->ref);
}
