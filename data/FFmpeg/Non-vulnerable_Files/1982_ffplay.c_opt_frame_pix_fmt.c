static int opt_frame_pix_fmt(void *optctx, const char *opt, const char *arg)
{
    av_log(NULL, AV_LOG_WARNING, "Option -pix_fmt is deprecated, use -pixel_format.\n");
    return opt_default(NULL, "pixel_format", arg);
}
