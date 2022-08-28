int av_opt_set_double(void *obj, const char *name, double val, int search_flags)
{
    return set_number(obj, name, val, 1, 1, search_flags);
}
