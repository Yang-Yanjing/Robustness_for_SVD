const AVOption *av_opt_next(FF_CONST_AVUTIL55 void *obj, const AVOption *last)
{
    const AVClass *class;
    if (!obj)
        return NULL;
    class = *(const AVClass**)obj;
    if (!last && class && class->option && class->option[0].name)
        return class->option;
    if (last && last[1].name)
        return ++last;
    return NULL;
}
