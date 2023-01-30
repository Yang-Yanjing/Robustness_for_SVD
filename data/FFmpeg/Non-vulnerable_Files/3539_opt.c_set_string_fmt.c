static int set_string_fmt(void *obj, const AVOption *o, const char *val, uint8_t *dst,
                          int fmt_nb, int ((*get_fmt)(const char *)), const char *desc)
{
    int fmt, min, max;
    if (!val || !strcmp(val, "none")) {
        fmt = -1;
    } else {
        fmt = get_fmt(val);
        if (fmt == -1) {
            char *tail;
            fmt = strtol(val, &tail, 0);
            if (*tail || (unsigned)fmt >= fmt_nb) {
                av_log(obj, AV_LOG_ERROR,
                       "Unable to parse option value \"%s\" as %s\n", val, desc);
                return AVERROR(EINVAL);
            }
        }
    }
    min = FFMAX(o->min, -1);
    max = FFMIN(o->max, fmt_nb-1);
    
    if(min == 0 && max == 0) {
        min = -1;
        max = fmt_nb-1;
    }
    if (fmt < min || fmt > max) {
        av_log(obj, AV_LOG_ERROR,
               "Value %d for parameter '%s' out of %s format range [%d - %d]\n",
               fmt, o->name, desc, min, max);
        return AVERROR(ERANGE);
    }
    *(int *)dst = fmt;
    return 0;
}
