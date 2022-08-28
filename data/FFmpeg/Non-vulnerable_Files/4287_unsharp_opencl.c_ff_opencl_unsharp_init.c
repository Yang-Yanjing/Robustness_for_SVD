int ff_opencl_unsharp_init(AVFilterContext *ctx)
{
    int ret = 0;
    char build_opts[96];
    UnsharpContext *unsharp = ctx->priv;
    ret = av_opencl_init(NULL);
    if (ret < 0)
        return ret;
    ret = av_opencl_buffer_create(&unsharp->opencl_ctx.cl_luma_mask,
                                  sizeof(uint32_t) * (2 * unsharp->luma.steps_x + 1) * (2 * unsharp->luma.steps_y + 1),
                                  CL_MEM_READ_ONLY, NULL);
    if (ret < 0)
        return ret;
    ret = av_opencl_buffer_create(&unsharp->opencl_ctx.cl_chroma_mask,
                                  sizeof(uint32_t) * (2 * unsharp->chroma.steps_x + 1) * (2 * unsharp->chroma.steps_y + 1),
                                  CL_MEM_READ_ONLY, NULL);
    
    if (ret < 0)
        return ret;
    ret = av_opencl_buffer_create(&unsharp->opencl_ctx.cl_luma_mask_x,
                                  sizeof(uint32_t) * (2 * unsharp->luma.steps_x + 1),
                                  CL_MEM_READ_ONLY, NULL);
    if (ret < 0)
        return ret;
    ret = av_opencl_buffer_create(&unsharp->opencl_ctx.cl_luma_mask_y,
                                  sizeof(uint32_t) * (2 * unsharp->luma.steps_y + 1),
                                  CL_MEM_READ_ONLY, NULL);
    if (ret < 0)
        return ret;
    ret = av_opencl_buffer_create(&unsharp->opencl_ctx.cl_chroma_mask_x,
                                  sizeof(uint32_t) * (2 * unsharp->chroma.steps_x + 1),
                                  CL_MEM_READ_ONLY, NULL);
    if (ret < 0)
        return ret;
    ret = av_opencl_buffer_create(&unsharp->opencl_ctx.cl_chroma_mask_y,
                                  sizeof(uint32_t) * (2 * unsharp->chroma.steps_y + 1),
                                  CL_MEM_READ_ONLY, NULL);
    if (ret < 0)
        return ret;
    ret = generate_mask(ctx);
    if (ret < 0)
        return ret;
    unsharp->opencl_ctx.plane_num = PLANE_NUM;
    unsharp->opencl_ctx.command_queue = av_opencl_get_command_queue();
    if (!unsharp->opencl_ctx.command_queue) {
        av_log(ctx, AV_LOG_ERROR, "Unable to get OpenCL command queue in filter 'unsharp'\n");
        return AVERROR(EINVAL);
    }
    snprintf(build_opts, 96, "-D LU_RADIUS_X=%d -D LU_RADIUS_Y=%d -D CH_RADIUS_X=%d -D CH_RADIUS_Y=%d",
            2*unsharp->luma.steps_x+1, 2*unsharp->luma.steps_y+1, 2*unsharp->chroma.steps_x+1, 2*unsharp->chroma.steps_y+1);
    unsharp->opencl_ctx.program = av_opencl_compile("unsharp", build_opts);
    if (!unsharp->opencl_ctx.program) {
        av_log(ctx, AV_LOG_ERROR, "OpenCL failed to compile program 'unsharp'\n");
        return AVERROR(EINVAL);
    }
    if (unsharp->opencl_ctx.use_fast_kernels) {
        if (!unsharp->opencl_ctx.kernel_luma) {
            unsharp->opencl_ctx.kernel_luma = clCreateKernel(unsharp->opencl_ctx.program, "unsharp_luma", &ret);
            if (ret != CL_SUCCESS) {
                av_log(ctx, AV_LOG_ERROR, "OpenCL failed to create kernel 'unsharp_luma'\n");
                return ret;
            }
        }
        if (!unsharp->opencl_ctx.kernel_chroma) {
            unsharp->opencl_ctx.kernel_chroma = clCreateKernel(unsharp->opencl_ctx.program, "unsharp_chroma", &ret);
            if (ret < 0) {
                av_log(ctx, AV_LOG_ERROR, "OpenCL failed to create kernel 'unsharp_chroma'\n");
                return ret;
            }
        }
    }
    else {
        if (!unsharp->opencl_ctx.kernel_default) {
            unsharp->opencl_ctx.kernel_default = clCreateKernel(unsharp->opencl_ctx.program, "unsharp_default", &ret);
            if (ret < 0) {
                av_log(ctx, AV_LOG_ERROR, "OpenCL failed to create kernel 'unsharp_default'\n");
                return ret;
            }
        }
    }
    return ret;
}
