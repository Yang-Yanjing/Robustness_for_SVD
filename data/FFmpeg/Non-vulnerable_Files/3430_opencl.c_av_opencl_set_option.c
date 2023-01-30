int av_opencl_set_option(const char *key, const char *val)
{
    int ret = init_opencl_mtx( );
    if (ret < 0)
        return ret;
    LOCK_OPENCL;
    if (!opencl_ctx.opt_init_flag) {
        av_opt_set_defaults(&opencl_ctx);
        opencl_ctx.opt_init_flag = 1;
    }
    ret = av_opt_set(&opencl_ctx, key, val, 0);
    UNLOCK_OPENCL;
    return ret;
}
