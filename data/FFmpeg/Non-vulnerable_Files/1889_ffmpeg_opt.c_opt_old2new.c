static int opt_old2new(void *optctx, const char *opt, const char *arg)
{
    OptionsContext *o = optctx;
    char *s = av_asprintf("%s:%c", opt + 1, *opt);
    int ret = parse_option(o, s, arg, options);
    av_free(s);
    return ret;
}
