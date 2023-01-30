int parse_option(void *optctx, const char *opt, const char *arg,
                 const OptionDef *options)
{
    const OptionDef *po;
    int ret;
    po = find_option(options, opt);
    if (!po->name && opt[0] == 'n' && opt[1] == 'o') {
        
        po = find_option(options, opt + 2);
        if ((po->name && (po->flags & OPT_BOOL)))
            arg = "0";
    } else if (po->flags & OPT_BOOL)
        arg = "1";
    if (!po->name)
        po = find_option(options, "default");
    if (!po->name) {
        av_log(NULL, AV_LOG_ERROR, "Unrecognized option '%s'\n", opt);
        return AVERROR(EINVAL);
    }
    if (po->flags & HAS_ARG && !arg) {
        av_log(NULL, AV_LOG_ERROR, "Missing argument for option '%s'\n", opt);
        return AVERROR(EINVAL);
    }
    ret = write_option(optctx, po, opt, arg);
    if (ret < 0)
        return ret;
    return !!(po->flags & HAS_ARG);
}
