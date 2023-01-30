static av_cold void uninit(AVFilterContext *ctx)
{
    UnsharpContext *unsharp = ctx->priv;
    if (CONFIG_OPENCL && unsharp->opencl) {
        ff_opencl_unsharp_uninit(ctx);
    }
    free_filter_param(&unsharp->luma);
    free_filter_param(&unsharp->chroma);
}
