static int set_format(void *obj, const char *name, int fmt, int search_flags,
                      enum AVOptionType type, const char *desc, int nb_fmts)
{
    void *target_obj;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0,
                                     search_flags, &target_obj);
    int min, max;
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != type) {
        av_log(obj, AV_LOG_ERROR,
               "The value set by option '%s' is not a %s format", name, desc);
        return AVERROR(EINVAL);
    }
    min = FFMAX(o->min, -1);
    max = FFMIN(o->max, nb_fmts-1);
    if (fmt < min || fmt > max) {
        av_log(obj, AV_LOG_ERROR,
               "Value %d for parameter '%s' out of %s format range [%d - %d]\n",
               fmt, name, desc, min, max);
        return AVERROR(ERANGE);
    }
    *(int *)(((uint8_t *)target_obj) + o->offset) = fmt;
    return 0;
}
