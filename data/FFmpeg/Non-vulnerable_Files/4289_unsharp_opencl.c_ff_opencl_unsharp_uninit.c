void ff_opencl_unsharp_uninit(AVFilterContext *ctx)
{
    UnsharpContext *unsharp = ctx->priv;
    av_opencl_buffer_release(&unsharp->opencl_ctx.cl_inbuf);
    av_opencl_buffer_release(&unsharp->opencl_ctx.cl_outbuf);
    av_opencl_buffer_release(&unsharp->opencl_ctx.cl_luma_mask);
    av_opencl_buffer_release(&unsharp->opencl_ctx.cl_chroma_mask);
    av_opencl_buffer_release(&unsharp->opencl_ctx.cl_luma_mask_x);
    av_opencl_buffer_release(&unsharp->opencl_ctx.cl_chroma_mask_x);
    av_opencl_buffer_release(&unsharp->opencl_ctx.cl_luma_mask_y);
    av_opencl_buffer_release(&unsharp->opencl_ctx.cl_chroma_mask_y);
    clReleaseKernel(unsharp->opencl_ctx.kernel_default);
    clReleaseKernel(unsharp->opencl_ctx.kernel_luma);
    clReleaseKernel(unsharp->opencl_ctx.kernel_chroma);
    clReleaseProgram(unsharp->opencl_ctx.program);
    unsharp->opencl_ctx.command_queue = NULL;
    av_opencl_uninit();
}
