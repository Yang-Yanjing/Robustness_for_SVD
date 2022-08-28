static int opt_qscale(void *optctx, const char *opt, const char *arg)
{
    OptionsContext *o = optctx;
    char *s;
    int ret;
    if(!strcmp(opt, "qscale")){
        av_log(NULL, AV_LOG_WARNING, "Please use -q:a or -q:v, -qscale is ambiguous\n");
        return parse_option(o, "q:v", arg, options);
    }
    s = av_asprintf("q%s", opt + 6);
    ret = parse_option(o, s, arg, options);
    av_free(s);
    return ret;
}
