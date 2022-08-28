static int get_number(void *obj, const char *name, const AVOption **o_out, double *num, int *den, int64_t *intnum,
                      int search_flags)
{
    void *dst, *target_obj;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        goto error;
    dst = ((uint8_t*)target_obj) + o->offset;
    if (o_out) *o_out= o;
    return read_number(o, dst, num, den, intnum);
error:
    *den=*intnum=0;
    return -1;
}
