const AVOption *av_set_double(void *obj, const char *name, double n)
{
    const AVOption *o = av_opt_find(obj, name, NULL, 0, 0);
    if (set_number(obj, name, n, 1, 1, 0) < 0)
        return NULL;
    return o;
}
