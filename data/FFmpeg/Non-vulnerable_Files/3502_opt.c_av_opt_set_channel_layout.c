int av_opt_set_channel_layout(void *obj, const char *name, int64_t cl, int search_flags)
{
    void *target_obj;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != AV_OPT_TYPE_CHANNEL_LAYOUT) {
        av_log(obj, AV_LOG_ERROR,
               "The value set by option '%s' is not a channel layout.\n", o->name);
        return AVERROR(EINVAL);
    }
    *(int64_t *)(((uint8_t *)target_obj) + o->offset) = cl;
    return 0;
}
