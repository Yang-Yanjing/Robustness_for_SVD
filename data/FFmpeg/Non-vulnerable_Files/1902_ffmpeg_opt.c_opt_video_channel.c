static int opt_video_channel(void *optctx, const char *opt, const char *arg)
{
    av_log(NULL, AV_LOG_WARNING, "This option is deprecated, use -channel.\n");
    return opt_default(optctx, "channel", arg);
}
