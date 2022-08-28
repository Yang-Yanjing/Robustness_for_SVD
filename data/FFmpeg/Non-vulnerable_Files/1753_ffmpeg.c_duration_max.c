
static AVRational duration_max(int64_t tmp, int64_t *duration, AVRational tmp_time_base,
                                AVRational time_base)
{
    int ret;
    if (!*duration) {
        *duration = tmp;
        return tmp_time_base;
    }
    ret = av_compare_ts(*duration, time_base, tmp, tmp_time_base);
    if (ret < 0) {
        *duration = tmp;
        return tmp_time_base;
    }
    return time_base;
}
