static int opt_video_codec(void *optctx, const char *opt, const char *arg)
{
    OptionsContext *o = optctx;
    return parse_option(o, "codec:v", arg, options);
}
