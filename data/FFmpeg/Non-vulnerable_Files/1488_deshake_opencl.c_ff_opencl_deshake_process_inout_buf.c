int ff_opencl_deshake_process_inout_buf(AVFilterContext *ctx, AVFrame *in, AVFrame *out)
{
    int ret = 0;
    AVFilterLink *link = ctx->inputs[0];
    DeshakeContext *deshake = ctx->priv;
    const int hshift = av_pix_fmt_desc_get(link->format)->log2_chroma_h;
    int chroma_height = FF_CEIL_RSHIFT(link->h, hshift);
    if ((!deshake->opencl_ctx.cl_inbuf) || (!deshake->opencl_ctx.cl_outbuf)) {
        deshake->opencl_ctx.in_plane_size[0]  = (in->linesize[0] * in->height);
        deshake->opencl_ctx.in_plane_size[1]  = (in->linesize[1] * chroma_height);
        deshake->opencl_ctx.in_plane_size[2]  = (in->linesize[2] * chroma_height);
        deshake->opencl_ctx.out_plane_size[0] = (out->linesize[0] * out->height);
        deshake->opencl_ctx.out_plane_size[1] = (out->linesize[1] * chroma_height);
        deshake->opencl_ctx.out_plane_size[2] = (out->linesize[2] * chroma_height);
        deshake->opencl_ctx.cl_inbuf_size  = deshake->opencl_ctx.in_plane_size[0] +
                                             deshake->opencl_ctx.in_plane_size[1] +
                                             deshake->opencl_ctx.in_plane_size[2];
        deshake->opencl_ctx.cl_outbuf_size = deshake->opencl_ctx.out_plane_size[0] +
                                             deshake->opencl_ctx.out_plane_size[1] +
                                             deshake->opencl_ctx.out_plane_size[2];
        if (!deshake->opencl_ctx.cl_inbuf) {
            ret = av_opencl_buffer_create(&deshake->opencl_ctx.cl_inbuf,
                                            deshake->opencl_ctx.cl_inbuf_size,
                                            CL_MEM_READ_ONLY, NULL);
            if (ret < 0)
                return ret;
        }
        if (!deshake->opencl_ctx.cl_outbuf) {
            ret = av_opencl_buffer_create(&deshake->opencl_ctx.cl_outbuf,
                                            deshake->opencl_ctx.cl_outbuf_size,
                                            CL_MEM_READ_WRITE, NULL);
            if (ret < 0)
                return ret;
        }
    }
    ret = av_opencl_buffer_write_image(deshake->opencl_ctx.cl_inbuf,
                                 deshake->opencl_ctx.cl_inbuf_size,
                                 0, in->data,deshake->opencl_ctx.in_plane_size,
                                 deshake->opencl_ctx.plane_num);
    if(ret < 0)
        return ret;
    return ret;
}
