int ff_opencl_apply_unsharp(AVFilterContext *ctx, AVFrame *in, AVFrame *out)
{
    int ret;
    AVFilterLink *link = ctx->inputs[0];
    UnsharpContext *unsharp = ctx->priv;
    cl_int status;
    FFOpenclParam kernel1 = {0};
    FFOpenclParam kernel2 = {0};
    int width = link->w;
    int height = link->h;
    int cw = FF_CEIL_RSHIFT(link->w, unsharp->hsub);
    int ch = FF_CEIL_RSHIFT(link->h, unsharp->vsub);
    size_t globalWorkSize1d = width * height + 2 * ch * cw;
    size_t globalWorkSize2dLuma[2];
    size_t globalWorkSize2dChroma[2];
    size_t localWorkSize2d[2] = {16, 16};
    if (unsharp->opencl_ctx.use_fast_kernels) {
        globalWorkSize2dLuma[0] = (size_t)ROUND_TO_16(width);
        globalWorkSize2dLuma[1] = (size_t)ROUND_TO_16(height);
        globalWorkSize2dChroma[0] = (size_t)ROUND_TO_16(cw);
        globalWorkSize2dChroma[1] = (size_t)(2*ROUND_TO_16(ch));
        kernel1.ctx = ctx;
        kernel1.kernel = unsharp->opencl_ctx.kernel_luma;
        ret = avpriv_opencl_set_parameter(&kernel1,
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_inbuf),
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_outbuf),
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_luma_mask_x),
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_luma_mask_y),
                                      FF_OPENCL_PARAM_INFO(unsharp->luma.amount),
                                      FF_OPENCL_PARAM_INFO(unsharp->luma.scalebits),
                                      FF_OPENCL_PARAM_INFO(unsharp->luma.halfscale),
                                      FF_OPENCL_PARAM_INFO(in->linesize[0]),
                                      FF_OPENCL_PARAM_INFO(out->linesize[0]),
                                      FF_OPENCL_PARAM_INFO(width),
                                      FF_OPENCL_PARAM_INFO(height),
                                      NULL);
        if (ret < 0)
            return ret;
        kernel2.ctx = ctx;
        kernel2.kernel = unsharp->opencl_ctx.kernel_chroma;
        ret = avpriv_opencl_set_parameter(&kernel2,
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_inbuf),
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_outbuf),
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_chroma_mask_x),
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_chroma_mask_y),
                                      FF_OPENCL_PARAM_INFO(unsharp->chroma.amount),
                                      FF_OPENCL_PARAM_INFO(unsharp->chroma.scalebits),
                                      FF_OPENCL_PARAM_INFO(unsharp->chroma.halfscale),
                                      FF_OPENCL_PARAM_INFO(in->linesize[0]),
                                      FF_OPENCL_PARAM_INFO(in->linesize[1]),
                                      FF_OPENCL_PARAM_INFO(out->linesize[0]),
                                      FF_OPENCL_PARAM_INFO(out->linesize[1]),
                                      FF_OPENCL_PARAM_INFO(link->w),
                                      FF_OPENCL_PARAM_INFO(link->h),
                                      FF_OPENCL_PARAM_INFO(cw),
                                      FF_OPENCL_PARAM_INFO(ch),
                                      NULL);
        if (ret < 0)
            return ret;
        status = clEnqueueNDRangeKernel(unsharp->opencl_ctx.command_queue,
                                        unsharp->opencl_ctx.kernel_luma, 2, NULL,
                                        globalWorkSize2dLuma, localWorkSize2d, 0, NULL, NULL);
        status |=clEnqueueNDRangeKernel(unsharp->opencl_ctx.command_queue,
                                        unsharp->opencl_ctx.kernel_chroma, 2, NULL,
                                        globalWorkSize2dChroma, localWorkSize2d, 0, NULL, NULL);
        if (status != CL_SUCCESS) {
            av_log(ctx, AV_LOG_ERROR, "OpenCL run kernel error occurred: %s\n", av_opencl_errstr(status));
            return AVERROR_EXTERNAL;
        }
    } else {    
        kernel1.ctx = ctx;
        kernel1.kernel = unsharp->opencl_ctx.kernel_default;
        ret = avpriv_opencl_set_parameter(&kernel1,
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_inbuf),
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_outbuf),
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_luma_mask),
                                      FF_OPENCL_PARAM_INFO(unsharp->opencl_ctx.cl_chroma_mask),
                                      FF_OPENCL_PARAM_INFO(unsharp->luma.amount),
                                      FF_OPENCL_PARAM_INFO(unsharp->chroma.amount),
                                      FF_OPENCL_PARAM_INFO(unsharp->luma.steps_x),
                                      FF_OPENCL_PARAM_INFO(unsharp->luma.steps_y),
                                      FF_OPENCL_PARAM_INFO(unsharp->chroma.steps_x),
                                      FF_OPENCL_PARAM_INFO(unsharp->chroma.steps_y),
                                      FF_OPENCL_PARAM_INFO(unsharp->luma.scalebits),
                                      FF_OPENCL_PARAM_INFO(unsharp->chroma.scalebits),
                                      FF_OPENCL_PARAM_INFO(unsharp->luma.halfscale),
                                      FF_OPENCL_PARAM_INFO(unsharp->chroma.halfscale),
                                      FF_OPENCL_PARAM_INFO(in->linesize[0]),
                                      FF_OPENCL_PARAM_INFO(in->linesize[1]),
                                      FF_OPENCL_PARAM_INFO(out->linesize[0]),
                                      FF_OPENCL_PARAM_INFO(out->linesize[1]),
                                      FF_OPENCL_PARAM_INFO(link->h),
                                      FF_OPENCL_PARAM_INFO(link->w),
                                      FF_OPENCL_PARAM_INFO(ch),
                                      FF_OPENCL_PARAM_INFO(cw),
                                      NULL);
        if (ret < 0)
            return ret;
        status = clEnqueueNDRangeKernel(unsharp->opencl_ctx.command_queue,
                                        unsharp->opencl_ctx.kernel_default, 1, NULL,
                                        &globalWorkSize1d, NULL, 0, NULL, NULL);
        if (status != CL_SUCCESS) {
            av_log(ctx, AV_LOG_ERROR, "OpenCL run kernel error occurred: %s\n", av_opencl_errstr(status));
            return AVERROR_EXTERNAL;
        }
    }
    
    
    return av_opencl_buffer_read_image(out->data, unsharp->opencl_ctx.out_plane_size,
                                       unsharp->opencl_ctx.plane_num, unsharp->opencl_ctx.cl_outbuf,
                                       unsharp->opencl_ctx.cl_outbuf_size);
}
