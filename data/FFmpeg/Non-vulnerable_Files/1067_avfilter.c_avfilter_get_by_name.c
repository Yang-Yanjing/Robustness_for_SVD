AVFilter *avfilter_get_by_name(const char *name)
{
    const AVFilter *f = NULL;
    if (!name)
        return NULL;
    while ((f = avfilter_next(f)))
        if (!strcmp(f->name, name))
            return (AVFilter *)f;
    return NULL;
}
