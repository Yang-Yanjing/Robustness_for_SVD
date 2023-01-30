static int str_to_time(const char *str, int64_t *rtime)
{
    const char *cur = str;
    char *end;
    int hours, minutes;
    double seconds = 0;
    if (*cur < '0' || *cur > '9')
        return 0;
    hours = strtol(cur, &end, 10);
    if (end == cur || *end != ':' || end[1] < '0' || end[1] > '9')
        return 0;
    cur = end + 1;
    minutes = strtol(cur, &end, 10);
    if (end == cur)
        return 0;
    cur = end;
    if (*end == ':'){
        seconds = strtod(cur + 1, &end);
        if (end > cur + 1)
            cur = end;
    }
    *rtime = (hours * 3600 + minutes * 60 + seconds) * AV_TIME_BASE;
    return cur - str;
}
