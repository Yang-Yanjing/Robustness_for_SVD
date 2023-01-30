int opt_cpuflags(void *optctx, const char *opt, const char *arg)
{
    int ret;
    unsigned flags = av_get_cpu_flags();
    if ((ret = av_parse_cpu_caps(&flags, arg)) < 0)
        return ret;
    av_force_cpu_flags(flags);
    return 0;
}
