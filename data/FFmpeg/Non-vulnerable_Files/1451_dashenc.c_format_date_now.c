static void format_date_now(char *buf, int size)
{
    time_t t = time(NULL);
    struct tm *ptm, tmbuf;
    ptm = gmtime_r(&t, &tmbuf);
    if (ptm) {
        if (!strftime(buf, size, "%Y-%m-%dT%H:%M:%S", ptm))
            buf[0] = '\0';
    }
}
