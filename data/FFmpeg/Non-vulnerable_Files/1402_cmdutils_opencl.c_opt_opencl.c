int opt_opencl(void *optctx, const char *opt, const char *arg)
{
    char *key, *value;
    const char *opts = arg;
    int ret = 0;
    while (*opts) {
        ret = av_opt_get_key_value(&opts, "=", ":", 0, &key, &value);
        if (ret < 0)
            return ret;
        ret = av_opencl_set_option(key, value);
        if (ret < 0)
            return ret;
        if (*opts)
            opts++;
    }
    return ret;
}
