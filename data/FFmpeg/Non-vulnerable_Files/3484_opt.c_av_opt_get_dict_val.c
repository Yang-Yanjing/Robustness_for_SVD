int av_opt_get_dict_val(void *obj, const char *name, int search_flags, AVDictionary **out_val)
{
    void *target_obj;
    AVDictionary *src;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != AV_OPT_TYPE_DICT)
        return AVERROR(EINVAL);
    src = *(AVDictionary **)(((uint8_t *)target_obj) + o->offset);
    av_dict_copy(out_val, src, 0);
    return 0;
}
