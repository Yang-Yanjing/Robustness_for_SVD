AVRational av_get_q(void *obj, const char *name, const AVOption **o_out)
{
    int64_t intnum=1;
    double num=1;
    int den=1;
    if (get_number(obj, name, o_out, &num, &den, &intnum, 0) < 0)
        return (AVRational){0, 0};
    if (num == 1.0 && (int)intnum == intnum)
        return (AVRational){intnum, den};
    else
        return av_d2q(num*intnum/den, 1<<24);
}
