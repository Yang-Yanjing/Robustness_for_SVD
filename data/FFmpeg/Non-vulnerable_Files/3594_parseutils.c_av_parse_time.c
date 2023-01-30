int av_parse_time(int64_t *timeval, const char *timestr, int duration)
{
    const char *p, *q;
    int64_t t;
    time_t now;
    struct tm dt = { 0 }, tmbuf;
    int today = 0, negative = 0, microseconds = 0;
    int i;
    static const char * const date_fmt[] = {
        "%Y-%m-%d",
        "%Y%m%d",
    };
    static const char * const time_fmt[] = {
        "%H:%M:%S",
        "%H%M%S",
    };
    p = timestr;
    q = NULL;
    *timeval = INT64_MIN;
    if (!duration) {
        now = time(0);
        if (!av_strcasecmp(timestr, "now")) {
            *timeval = (int64_t) now * 1000000;
            return 0;
        }
        
        for (i = 0; i < FF_ARRAY_ELEMS(date_fmt); i++) {
            q = av_small_strptime(p, date_fmt[i], &dt);
            if (q)
                break;
        }
        

        if (!q) {
            today = 1;
            q = p;
        }
        p = q;
        if (*p == 'T' || *p == 't' || *p == ' ')
            p++;
        
        for (i = 0; i < FF_ARRAY_ELEMS(time_fmt); i++) {
            q = av_small_strptime(p, time_fmt[i], &dt);
            if (q)
                break;
        }
    } else {
        
        if (p[0] == '-') {
            negative = 1;
            ++p;
        }
        
        q = av_small_strptime(p, "%J:%M:%S", &dt);
        if (!q) {
            
            q = av_small_strptime(p, "%M:%S", &dt);
            dt.tm_hour = 0;
        }
        if (!q) {
            char *o;
            
            dt.tm_sec = strtol(p, &o, 10);
            if (o == p) 
                return AVERROR(EINVAL);
            dt.tm_min = 0;
            dt.tm_hour = 0;
            q = o;
        }
    }
    
    if (!q)
        return AVERROR(EINVAL);
    
    if (*q == '.') {
        int n;
        q++;
        for (n = 100000; n >= 1; n /= 10, q++) {
            if (!av_isdigit(*q))
                break;
            microseconds += n * (*q - '0');
        }
        while (av_isdigit(*q))
            q++;
    }
    if (duration) {
        t = dt.tm_hour * 3600 + dt.tm_min * 60 + dt.tm_sec;
    } else {
        int is_utc = *q == 'Z' || *q == 'z';
        q += is_utc;
        if (today) { 
            struct tm dt2 = is_utc ? *gmtime_r(&now, &tmbuf) : *localtime_r(&now, &tmbuf);
            dt2.tm_hour = dt.tm_hour;
            dt2.tm_min  = dt.tm_min;
            dt2.tm_sec  = dt.tm_sec;
            dt = dt2;
        }
        t = is_utc ? av_timegm(&dt) : mktime(&dt);
    }
    
    if (*q)
        return AVERROR(EINVAL);
    t *= 1000000;
    t += microseconds;
    *timeval = negative ? -t : t;
    return 0;
}
