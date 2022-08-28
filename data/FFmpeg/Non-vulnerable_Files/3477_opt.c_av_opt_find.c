const AVOption *av_opt_find(void *obj, const char *name, const char *unit,
                            int opt_flags, int search_flags)
{
    return av_opt_find2(obj, name, unit, opt_flags, search_flags, NULL);
}
