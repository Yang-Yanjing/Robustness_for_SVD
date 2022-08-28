static int opt_video_filters(void *optctx, const char *opt, const char *arg)
{
    OptionsContext *o = optctx;
    return parse_option(o, "filter:v", arg, options);
}
