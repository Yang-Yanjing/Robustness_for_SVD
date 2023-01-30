const AVOption *av_set_int(void *obj, const char *name, int64_t n)
{
    const AVOption *o = av_opt_find(obj, name, NULL, 0, 0);
    if (set_number(obj, name, 1, 1, n, 0) < 0)
        return NULL;
    return o;
}
