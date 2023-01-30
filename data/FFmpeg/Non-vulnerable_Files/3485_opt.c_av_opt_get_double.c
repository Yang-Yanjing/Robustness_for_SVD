int av_opt_get_double(void *obj, const char *name, int search_flags, double *out_val)
{
    int64_t intnum = 1;
    double     num = 1;
    int   ret, den = 1;
    if ((ret = get_number(obj, name, NULL, &num, &den, &intnum, search_flags)) < 0)
        return ret;
    *out_val = num*intnum/den;
    return 0;
}
