void ff_opencl_deshake_uninit(AVFilterContext *ctx)
{
    DeshakeContext *deshake = ctx->priv;
    av_opencl_buffer_release(&deshake->opencl_ctx.cl_inbuf);
    av_opencl_buffer_release(&deshake->opencl_ctx.cl_outbuf);
    clReleaseKernel(deshake->opencl_ctx.kernel_luma);
    clReleaseKernel(deshake->opencl_ctx.kernel_chroma);
    clReleaseProgram(deshake->opencl_ctx.program);
    deshake->opencl_ctx.command_queue = NULL;
    av_opencl_uninit();
}
