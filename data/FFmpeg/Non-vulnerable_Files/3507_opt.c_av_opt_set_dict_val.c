int av_opt_set_dict_val(void *obj, const char *name, const AVDictionary *val, int search_flags)
{
    void *target_obj;
    AVDictionary **dst;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->flags & AV_OPT_FLAG_READONLY)
        return AVERROR(EINVAL);
    dst = (AVDictionary **)(((uint8_t *)target_obj) + o->offset);
    av_dict_free(dst);
    av_dict_copy(dst, val, 0);
    return 0;
}
