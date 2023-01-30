int ff_opencl_unsharp_process_inout_buf(AVFilterContext *ctx, AVFrame *in, AVFrame *out)
{
    int ret = 0;
    AVFilterLink *link = ctx->inputs[0];
    UnsharpContext *unsharp = ctx->priv;
    int ch = FF_CEIL_RSHIFT(link->h, unsharp->vsub);
    if ((!unsharp->opencl_ctx.cl_inbuf) || (!unsharp->opencl_ctx.cl_outbuf)) {
        unsharp->opencl_ctx.in_plane_size[0]  = (in->linesize[0] * in->height);
        unsharp->opencl_ctx.in_plane_size[1]  = (in->linesize[1] * ch);
        unsharp->opencl_ctx.in_plane_size[2]  = (in->linesize[2] * ch);
        unsharp->opencl_ctx.out_plane_size[0] = (out->linesize[0] * out->height);
        unsharp->opencl_ctx.out_plane_size[1] = (out->linesize[1] * ch);
        unsharp->opencl_ctx.out_plane_size[2] = (out->linesize[2] * ch);
        unsharp->opencl_ctx.cl_inbuf_size  = unsharp->opencl_ctx.in_plane_size[0] +
                                             unsharp->opencl_ctx.in_plane_size[1] +
                                             unsharp->opencl_ctx.in_plane_size[2];
        unsharp->opencl_ctx.cl_outbuf_size = unsharp->opencl_ctx.out_plane_size[0] +
                                             unsharp->opencl_ctx.out_plane_size[1] +
                                             unsharp->opencl_ctx.out_plane_size[2];
        if (!unsharp->opencl_ctx.cl_inbuf) {
            ret = av_opencl_buffer_create(&unsharp->opencl_ctx.cl_inbuf,
                                          unsharp->opencl_ctx.cl_inbuf_size,
                                          CL_MEM_READ_ONLY, NULL);
            if (ret < 0)
                return ret;
        }
        if (!unsharp->opencl_ctx.cl_outbuf) {
            ret = av_opencl_buffer_create(&unsharp->opencl_ctx.cl_outbuf,
                                          unsharp->opencl_ctx.cl_outbuf_size,
                                          CL_MEM_READ_WRITE, NULL);
            if (ret < 0)
                return ret;
        }
    }
    return av_opencl_buffer_write_image(unsharp->opencl_ctx.cl_inbuf,
                                        unsharp->opencl_ctx.cl_inbuf_size,
                                        0, in->data, unsharp->opencl_ctx.in_plane_size,
                                        unsharp->opencl_ctx.plane_num);
}
