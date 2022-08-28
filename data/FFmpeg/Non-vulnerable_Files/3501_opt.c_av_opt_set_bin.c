int av_opt_set_bin(void *obj, const char *name, const uint8_t *val, int len, int search_flags)
{
    void *target_obj;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    uint8_t *ptr;
    uint8_t **dst;
    int *lendst;
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->type != AV_OPT_TYPE_BINARY || o->flags & AV_OPT_FLAG_READONLY)
        return AVERROR(EINVAL);
    ptr = len ? av_malloc(len) : NULL;
    if (len && !ptr)
        return AVERROR(ENOMEM);
    dst = (uint8_t **)(((uint8_t *)target_obj) + o->offset);
    lendst = (int *)(dst + 1);
    av_free(*dst);
    *dst = ptr;
    *lendst = len;
    if (len)
        memcpy(ptr, val, len);
    return 0;
}
