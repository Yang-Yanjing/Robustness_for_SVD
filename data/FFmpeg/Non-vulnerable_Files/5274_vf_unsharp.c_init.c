static av_cold int init(AVFilterContext *ctx)
{
    int ret = 0;
    UnsharpContext *unsharp = ctx->priv;
    set_filter_param(&unsharp->luma,   unsharp->lmsize_x, unsharp->lmsize_y, unsharp->lamount);
    set_filter_param(&unsharp->chroma, unsharp->cmsize_x, unsharp->cmsize_y, unsharp->camount);
    unsharp->apply_unsharp = apply_unsharp_c;
    if (!CONFIG_OPENCL && unsharp->opencl) {
        av_log(ctx, AV_LOG_ERROR, "OpenCL support was not enabled in this build, cannot be selected\n");
        return AVERROR(EINVAL);
    }
    if (CONFIG_OPENCL && unsharp->opencl) {
        unsharp->apply_unsharp = ff_opencl_apply_unsharp;
        ret = ff_opencl_unsharp_init(ctx);
        if (ret < 0)
            return ret;
    }
    return 0;
}
