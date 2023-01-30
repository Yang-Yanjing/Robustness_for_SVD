static int crazytime_to_iso8601(char *buf, int buf_size, int64_t value)
{
    time_t t = (value / 10000000LL) - 719162LL*86400LL;
    struct tm tmbuf;
    struct tm *tm = gmtime_r(&t, &tmbuf);
    if (!tm)
        return -1;
    if (!strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", tm))
        return -1;
    return 0;
}
