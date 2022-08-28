int64_t av_get_int(void *obj, const char *name, const AVOption **o_out)
{
    int64_t intnum=1;
    double num=1;
    int den=1;
    if (get_number(obj, name, o_out, &num, &den, &intnum, 0) < 0)
        return -1;
    return num*intnum/den;
}
