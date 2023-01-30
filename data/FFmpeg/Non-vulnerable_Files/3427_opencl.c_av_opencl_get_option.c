int av_opencl_get_option(const char *key, uint8_t **out_val)
{
    int ret = 0;
    LOCK_OPENCL;
    ret = av_opt_get(&opencl_ctx, key, 0, out_val);
    UNLOCK_OPENCL;
    return ret;
}
