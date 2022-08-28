static int generate_mask(AVFilterContext *ctx)
{
    cl_mem masks[4];
    cl_mem mask_matrix[2];
    int i, ret = 0, step_x[2], step_y[2];
    UnsharpContext *unsharp = ctx->priv;
    mask_matrix[0] = unsharp->opencl_ctx.cl_luma_mask;
    mask_matrix[1] = unsharp->opencl_ctx.cl_chroma_mask;
    masks[0] = unsharp->opencl_ctx.cl_luma_mask_x;
    masks[1] = unsharp->opencl_ctx.cl_luma_mask_y;
    masks[2] = unsharp->opencl_ctx.cl_chroma_mask_x;
    masks[3] = unsharp->opencl_ctx.cl_chroma_mask_y;
    step_x[0] = unsharp->luma.steps_x;
    step_x[1] = unsharp->chroma.steps_x;
    step_y[0] = unsharp->luma.steps_y;
    step_y[1] = unsharp->chroma.steps_y;
    
    if (step_x[0]>8 || step_x[1]>8 || step_y[0]>8 || step_y[1]>8)
        unsharp->opencl_ctx.use_fast_kernels = 0;
    else
        unsharp->opencl_ctx.use_fast_kernels = 1;
    if (!masks[0] || !masks[1] || !masks[2] || !masks[3]) {
        av_log(ctx, AV_LOG_ERROR, "Luma mask and chroma mask should not be NULL\n");
        return AVERROR(EINVAL);
    }
    if (!mask_matrix[0] || !mask_matrix[1]) {
        av_log(ctx, AV_LOG_ERROR, "Luma mask and chroma mask should not be NULL\n");
        return AVERROR(EINVAL);
    }
    for (i = 0; i < 2; i++) {
        ret = copy_separable_masks(masks[2*i], masks[2*i+1], step_x[i], step_y[i]);
        if (ret < 0)
            return ret;
    }
    return ret;
}
