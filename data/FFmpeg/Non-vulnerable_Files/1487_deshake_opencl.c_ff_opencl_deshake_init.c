int ff_opencl_deshake_init(AVFilterContext *ctx)
{
    int ret = 0;
    DeshakeContext *deshake = ctx->priv;
    ret = av_opencl_init(NULL);
    if (ret < 0)
        return ret;
    deshake->opencl_ctx.plane_num = PLANE_NUM;
    deshake->opencl_ctx.command_queue = av_opencl_get_command_queue();
    if (!deshake->opencl_ctx.command_queue) {
        av_log(ctx, AV_LOG_ERROR, "Unable to get OpenCL command queue in filter 'deshake'\n");
        return AVERROR(EINVAL);
    }
    deshake->opencl_ctx.program = av_opencl_compile("avfilter_transform", NULL);
    if (!deshake->opencl_ctx.program) {
        av_log(ctx, AV_LOG_ERROR, "OpenCL failed to compile program 'avfilter_transform'\n");
        return AVERROR(EINVAL);
    }
    if (!deshake->opencl_ctx.kernel_luma) {
        deshake->opencl_ctx.kernel_luma = clCreateKernel(deshake->opencl_ctx.program,
                                                         "avfilter_transform_luma", &ret);
        if (ret != CL_SUCCESS) {
            av_log(ctx, AV_LOG_ERROR, "OpenCL failed to create kernel 'avfilter_transform_luma'\n");
            return AVERROR(EINVAL);
        }
    }
    if (!deshake->opencl_ctx.kernel_chroma) {
        deshake->opencl_ctx.kernel_chroma = clCreateKernel(deshake->opencl_ctx.program,
                                                           "avfilter_transform_chroma", &ret);
        if (ret != CL_SUCCESS) {
            av_log(ctx, AV_LOG_ERROR, "OpenCL failed to create kernel 'avfilter_transform_chroma'\n");
            return AVERROR(EINVAL);
        }
    }
    return ret;
}
