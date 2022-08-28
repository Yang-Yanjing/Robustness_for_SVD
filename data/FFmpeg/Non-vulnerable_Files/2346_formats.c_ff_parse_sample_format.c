int ff_parse_sample_format(int *ret, const char *arg, void *log_ctx)
{
    char *tail;
    int sfmt = av_get_sample_fmt(arg);
    if (sfmt == AV_SAMPLE_FMT_NONE) {
        sfmt = strtol(arg, &tail, 0);
        if (*tail || av_get_bytes_per_sample(sfmt)<=0) {
            av_log(log_ctx, AV_LOG_ERROR, "Invalid sample format '%s'\n", arg);
            return AVERROR(EINVAL);
        }
    }
    *ret = sfmt;
    return 0;
}
