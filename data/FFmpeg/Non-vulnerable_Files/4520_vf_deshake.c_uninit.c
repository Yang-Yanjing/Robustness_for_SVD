static av_cold void uninit(AVFilterContext *ctx)
{
    DeshakeContext *deshake = ctx->priv;
    if (CONFIG_OPENCL && deshake->opencl) {
        ff_opencl_deshake_uninit(ctx);
    }
    av_frame_free(&deshake->ref);
    av_freep(&deshake->angles);
    deshake->angles_size = 0;
    if (deshake->fp)
        fclose(deshake->fp);
}
