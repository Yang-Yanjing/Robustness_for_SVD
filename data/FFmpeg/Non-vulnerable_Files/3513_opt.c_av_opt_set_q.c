int av_opt_set_q(void *obj, const char *name, AVRational val, int search_flags)
{
    return set_number(obj, name, val.num, val.den, 1, search_flags);
}
