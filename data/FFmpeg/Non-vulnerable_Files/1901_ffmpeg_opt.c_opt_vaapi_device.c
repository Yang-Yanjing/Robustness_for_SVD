static int opt_vaapi_device(void *optctx, const char *opt, const char *arg)
{
    int err;
    err = vaapi_device_init(arg);
    if (err < 0)
        exit_program(1);
    return 0;
}
