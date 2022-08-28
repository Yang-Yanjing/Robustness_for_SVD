static inline int scale_double(void *log, double d, double m, int *r)
{
    m *= d * SBG_SCALE;
    if (m < INT_MIN || m >= INT_MAX) {
        if (log)
            av_log(log, AV_LOG_ERROR, "%g is too large\n", d);
        return AVERROR(EDOM);
    }
    *r = m;
    return 0;
}
