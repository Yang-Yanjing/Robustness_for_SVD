double av_get_double(void *obj, const char *name, const AVOption **o_out)
{
    int64_t intnum=1;
    double num=1;
    int den=1;
    if (get_number(obj, name, o_out, &num, &den, &intnum, 0) < 0)
        return NAN;
    return num*intnum/den;
}
