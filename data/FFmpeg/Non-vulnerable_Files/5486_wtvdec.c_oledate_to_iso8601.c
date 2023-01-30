static int oledate_to_iso8601(char *buf, int buf_size, int64_t value)
{
    time_t t = (av_int2double(value) - 25569.0) * 86400;
    struct tm tmbuf;
    struct tm *tm= gmtime_r(&t, &tmbuf);
    if (!tm)
        return -1;
    if (!strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", tm))
        return -1;
    return 0;
}
