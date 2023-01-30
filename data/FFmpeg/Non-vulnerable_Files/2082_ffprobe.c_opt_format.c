static int opt_format(void *optctx, const char *opt, const char *arg)
{
    iformat = av_find_input_format(arg);
    if (!iformat) {
        av_log(NULL, AV_LOG_ERROR, "Unknown input format: %s\n", arg);
        return AVERROR(EINVAL);
    }
    return 0;
}
