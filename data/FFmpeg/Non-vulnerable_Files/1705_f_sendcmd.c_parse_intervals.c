static int parse_intervals(Interval **intervals, int *nb_intervals,
                           const char *buf, void *log_ctx)
{
    int interval_count = 0;
    int ret, n = 0;
    *intervals = NULL;
    *nb_intervals = 0;
    if (!buf)
        return 0;
    while (1) {
        Interval interval;
        skip_comments(&buf);
        if (!(*buf))
            break;
        if ((ret = parse_interval(&interval, interval_count, &buf, log_ctx)) < 0)
            return ret;
        buf += strspn(buf, SPACES);
        if (*buf) {
            if (*buf != ';') {
                av_log(log_ctx, AV_LOG_ERROR,
                       "Missing terminator or extraneous data found at the end of interval #%d\n",
                       interval_count);
                return AVERROR(EINVAL);
            }
            buf++; 
        }
        interval_count++;
        
        if (*nb_intervals == n) {
            n = FFMAX(16, 2*n); 
            *intervals = av_realloc_f(*intervals, n, 2*sizeof(Interval));
            if (!*intervals) {
                av_log(log_ctx, AV_LOG_ERROR,
                       "Could not (re)allocate intervals array\n");
                return AVERROR(ENOMEM);
            }
        }
        (*intervals)[(*nb_intervals)++] = interval;
    }
    return 0;
}
