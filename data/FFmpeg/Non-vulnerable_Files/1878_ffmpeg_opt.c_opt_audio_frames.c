static int opt_audio_frames(void *optctx, const char *opt, const char *arg)
{
    OptionsContext *o = optctx;
    return parse_option(o, "frames:a", arg, options);
}
