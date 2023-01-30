const AVOption *av_set_q(void *obj, const char *name, AVRational n)
{
    const AVOption *o = av_opt_find(obj, name, NULL, 0, 0);
    if (set_number(obj, name, n.num, n.den, 1, 0) < 0)
        return NULL;
    return o;
}
