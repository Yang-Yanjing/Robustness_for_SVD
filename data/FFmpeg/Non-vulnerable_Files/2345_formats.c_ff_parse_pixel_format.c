int ff_parse_pixel_format(enum AVPixelFormat *ret, const char *arg, void *log_ctx)
{
    char *tail;
    int pix_fmt = av_get_pix_fmt(arg);
    if (pix_fmt == AV_PIX_FMT_NONE) {
        pix_fmt = strtol(arg, &tail, 0);
        if (*tail || !av_pix_fmt_desc_get(pix_fmt)) {
            av_log(log_ctx, AV_LOG_ERROR, "Invalid pixel format '%s'\n", arg);
            return AVERROR(EINVAL);
        }
    }
    *ret = pix_fmt;
    return 0;
}
