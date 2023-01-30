int av_opt_set_video_rate(void *obj, const char *name, AVRational val, int search_flags)
{
    void *target_obj;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != AV_OPT_TYPE_VIDEO_RATE) {
        av_log(obj, AV_LOG_ERROR,
               "The value set by option '%s' is not a video rate.\n", o->name);
        return AVERROR(EINVAL);
    }
    if (val.num <= 0 || val.den <= 0)
        return AVERROR(EINVAL);
    return set_number(obj, name, val.num, val.den, 1, search_flags);
}
