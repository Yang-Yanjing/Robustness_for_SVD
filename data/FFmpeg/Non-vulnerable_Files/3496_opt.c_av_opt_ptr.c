void *av_opt_ptr(const AVClass *class, void *obj, const char *name)
{
    const AVOption *opt= av_opt_find2(&class, name, NULL, 0, AV_OPT_SEARCH_FAKE_OBJ, NULL);
    if(!opt)
        return NULL;
    return (uint8_t*)obj + opt->offset;
}
