int ff_parse_time_base(AVRational *ret, const char *arg, void *log_ctx)
{
    AVRational r;
    if(av_parse_ratio(&r, arg, INT_MAX, 0, log_ctx) < 0 ||r.num<=0  ||r.den<=0) {
        av_log(log_ctx, AV_LOG_ERROR, "Invalid time base '%s'\n", arg);
        return AVERROR(EINVAL);
    }
    *ret = r;
    return 0;
}
