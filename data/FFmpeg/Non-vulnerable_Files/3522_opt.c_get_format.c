static int get_format(void *obj, const char *name, int search_flags, int *out_fmt,
                      enum AVOptionType type, const char *desc)
{
    void *dst, *target_obj;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != type) {
        av_log(obj, AV_LOG_ERROR,
               "The value for option '%s' is not a %s format.\n", desc, name);
        return AVERROR(EINVAL);
    }
    dst = ((uint8_t*)target_obj) + o->offset;
    *out_fmt = *(int *)dst;
    return 0;
}
