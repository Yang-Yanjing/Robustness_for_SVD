int ff_opencl_transform(AVFilterContext *ctx,
                        int width, int height, int cw, int ch,
                        const float *matrix_y, const float *matrix_uv,
                        enum InterpolateMethod interpolate,
                        enum FillMethod fill, AVFrame *in, AVFrame *out)
{
    int ret = 0;
    cl_int status;
    DeshakeContext *deshake = ctx->priv;
    float4 packed_matrix_lu = {matrix_y[0], matrix_y[1], matrix_y[2], matrix_y[5]};
    float4 packed_matrix_ch = {matrix_uv[0], matrix_uv[1], matrix_uv[2], matrix_uv[5]};
    size_t global_worksize_lu[2] = {(size_t)ROUND_TO_16(width), (size_t)ROUND_TO_16(height)};
    size_t global_worksize_ch[2] = {(size_t)ROUND_TO_16(cw), (size_t)(2*ROUND_TO_16(ch))};
    size_t local_worksize[2] = {16, 16};
    FFOpenclParam param_lu = {0};
    FFOpenclParam param_ch = {0};
    param_lu.ctx = param_ch.ctx = ctx;
    param_lu.kernel = deshake->opencl_ctx.kernel_luma;
    param_ch.kernel = deshake->opencl_ctx.kernel_chroma;
    if ((unsigned int)interpolate > INTERPOLATE_BIQUADRATIC) {
        av_log(ctx, AV_LOG_ERROR, "Selected interpolate method is invalid\n");
        return AVERROR(EINVAL);
    }
    ret = avpriv_opencl_set_parameter(&param_lu,
                                  FF_OPENCL_PARAM_INFO(deshake->opencl_ctx.cl_inbuf),
                                  FF_OPENCL_PARAM_INFO(deshake->opencl_ctx.cl_outbuf),
                                  FF_OPENCL_PARAM_INFO(packed_matrix_lu),
                                  FF_OPENCL_PARAM_INFO(interpolate),
                                  FF_OPENCL_PARAM_INFO(fill),
                                  FF_OPENCL_PARAM_INFO(in->linesize[0]),
                                  FF_OPENCL_PARAM_INFO(out->linesize[0]),
                                  FF_OPENCL_PARAM_INFO(height),
                                  FF_OPENCL_PARAM_INFO(width),
                                  NULL);
    if (ret < 0)
        return ret;
    ret = avpriv_opencl_set_parameter(&param_ch,
                                  FF_OPENCL_PARAM_INFO(deshake->opencl_ctx.cl_inbuf),
                                  FF_OPENCL_PARAM_INFO(deshake->opencl_ctx.cl_outbuf),
                                  FF_OPENCL_PARAM_INFO(packed_matrix_ch),
                                  FF_OPENCL_PARAM_INFO(interpolate),
                                  FF_OPENCL_PARAM_INFO(fill),
                                  FF_OPENCL_PARAM_INFO(in->linesize[0]),
                                  FF_OPENCL_PARAM_INFO(out->linesize[0]),
                                  FF_OPENCL_PARAM_INFO(in->linesize[1]),
                                  FF_OPENCL_PARAM_INFO(out->linesize[1]),
                                  FF_OPENCL_PARAM_INFO(height),
                                  FF_OPENCL_PARAM_INFO(width),
                                  FF_OPENCL_PARAM_INFO(ch),
                                  FF_OPENCL_PARAM_INFO(cw),
                                  NULL);
    if (ret < 0)
        return ret;
    status = clEnqueueNDRangeKernel(deshake->opencl_ctx.command_queue,
                                    deshake->opencl_ctx.kernel_luma, 2, NULL,
                                    global_worksize_lu, local_worksize, 0, NULL, NULL);
    status |= clEnqueueNDRangeKernel(deshake->opencl_ctx.command_queue,
                                    deshake->opencl_ctx.kernel_chroma, 2, NULL,
                                    global_worksize_ch, local_worksize, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        av_log(ctx, AV_LOG_ERROR, "OpenCL run kernel error occurred: %s\n", av_opencl_errstr(status));
        return AVERROR_EXTERNAL;
    }
    ret = av_opencl_buffer_read_image(out->data, deshake->opencl_ctx.out_plane_size,
                                      deshake->opencl_ctx.plane_num, deshake->opencl_ctx.cl_outbuf,
                                      deshake->opencl_ctx.cl_outbuf_size);
    if (ret < 0)
        return ret;
    return ret;
}
