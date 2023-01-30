int av_opt_query_ranges(AVOptionRanges **ranges_arg, void *obj, const char *key, int flags)
{
    int ret;
    const AVClass *c = *(AVClass**)obj;
    int (*callback)(AVOptionRanges **, void *obj, const char *key, int flags) = NULL;
    if (c->version > (52 << 16 | 11 << 8))
        callback = c->query_ranges;
    if (!callback)
        callback = av_opt_query_ranges_default;
    ret = callback(ranges_arg, obj, key, flags);
    if (ret >= 0) {
        if (!(flags & AV_OPT_MULTI_COMPONENT_RANGE))
            ret = 1;
        (*ranges_arg)->nb_components = ret;
    }
    return ret;
}
