OPT_EVAL_NUMBER(q,      AV_OPT_TYPE_RATIONAL, AVRational)
static int set_number(void *obj, const char *name, double num, int den, int64_t intnum,
                                  int search_flags)
{
    void *dst, *target_obj;
    const AVOption *o = av_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return AVERROR_OPTION_NOT_FOUND;
    if (o->flags & AV_OPT_FLAG_READONLY)
        return AVERROR(EINVAL);
    dst = ((uint8_t*)target_obj) + o->offset;
    return write_number(obj, o, dst, num, den, intnum);
}
