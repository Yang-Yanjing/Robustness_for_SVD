int av_opencl_init(AVOpenCLExternalEnv *ext_opencl_env)
{
    int ret = init_opencl_mtx( );
    if (ret < 0)
        return ret;
    LOCK_OPENCL;
    if (!opencl_ctx.init_count) {
        if (!opencl_ctx.opt_init_flag) {
            av_opt_set_defaults(&opencl_ctx);
            opencl_ctx.opt_init_flag = 1;
        }
        ret = init_opencl_env(&opencl_ctx, ext_opencl_env);
        if (ret < 0)
            goto end;
        if (opencl_ctx.kernel_code_count <= 0) {
            av_log(&opencl_ctx, AV_LOG_ERROR,
                   "No kernel code is registered, compile kernel file failed\n");
            ret = AVERROR(EINVAL);
            goto end;
        }
    }
    opencl_ctx.init_count++;
end:
    UNLOCK_OPENCL;
    return ret;
}
