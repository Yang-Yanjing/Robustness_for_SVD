int av_opt_get_channel_layout(void *obj, const char *name, int search_flags, int64_t *cl)
{
    void *dst, *target_obj;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != AV_OPT_TYPE_CHANNEL_LAYOUT) {
        av_log(obj, AV_LOG_ERROR,
               "The value for option '%s' is not a channel layout.\n", name);
        return AVERROR(EINVAL);
    }
    dst = ((uint8_t*)target_obj) + o->offset;
    *cl = *(int64_t *)dst;
    return 0;
}
