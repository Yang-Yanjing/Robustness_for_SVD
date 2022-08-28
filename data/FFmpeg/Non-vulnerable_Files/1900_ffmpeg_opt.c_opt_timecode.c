static int opt_timecode(void *optctx, const char *opt, const char *arg)
{
    OptionsContext *o = optctx;
    char *tcr = av_asprintf("timecode=%s", arg);
    int ret = parse_option(o, "metadata:g", tcr, options);
    if (ret >= 0)
        ret = av_dict_set(&o->g->codec_opts, "gop_timecode", arg, 0);
    av_free(tcr);
    return 0;
}
