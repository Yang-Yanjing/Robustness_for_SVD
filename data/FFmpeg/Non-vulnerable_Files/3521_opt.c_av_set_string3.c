int av_set_string3(void *obj, const char *name, const char *val, int alloc, const AVOption **o_out)
{
    const AVOption *o = av_opt_find(obj, name, NULL, 0, 0);
    if (o_out)
        *o_out = o;
    return av_opt_set(obj, name, val, 0);
}
