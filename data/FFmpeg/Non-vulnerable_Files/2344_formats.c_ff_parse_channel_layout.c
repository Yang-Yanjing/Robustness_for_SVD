int ff_parse_channel_layout(int64_t *ret, int *nret, const char *arg,
                            void *log_ctx)
{
    char *tail;
    int64_t chlayout;
    chlayout = av_get_channel_layout(arg);
    if (chlayout == 0) {
        chlayout = strtol(arg, &tail, 10);
        if (!(*tail == '\0' || *tail == 'c' && *(tail + 1) == '\0') || chlayout <= 0 || chlayout > 63) {
            av_log(log_ctx, AV_LOG_ERROR, "Invalid channel layout '%s'\n", arg);
            return AVERROR(EINVAL);
        }
        if (nret) {
            *nret = chlayout;
            *ret = 0;
            return 0;
        }
    }
    *ret = chlayout;
    if (nret)
        *nret = av_get_channel_layout_nb_channels(chlayout);
    return 0;
}
