static int opt_video_standard(void *optctx, const char *opt, const char *arg)
{
    av_log(NULL, AV_LOG_WARNING, "This option is deprecated, use -standard.\n");
    return opt_default(optctx, "standard", arg);
}
