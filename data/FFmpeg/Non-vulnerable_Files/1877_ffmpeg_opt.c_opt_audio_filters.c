static int opt_audio_filters(void *optctx, const char *opt, const char *arg)
{
    OptionsContext *o = optctx;
    return parse_option(o, "filter:a", arg, options);
}
