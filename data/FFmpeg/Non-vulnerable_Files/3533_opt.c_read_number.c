static int read_number(const AVOption *o, const void *dst, double *num, int *den, int64_t *intnum)
{
    switch (o->type) {
    case AV_OPT_TYPE_FLAGS:     *intnum = *(unsigned int*)dst;return 0;
    case AV_OPT_TYPE_PIXEL_FMT: *intnum = *(enum AVPixelFormat *)dst;return 0;
    case AV_OPT_TYPE_SAMPLE_FMT:*intnum = *(enum AVSampleFormat*)dst;return 0;
    case AV_OPT_TYPE_INT:       *intnum = *(int         *)dst;return 0;
    case AV_OPT_TYPE_CHANNEL_LAYOUT:
    case AV_OPT_TYPE_DURATION:
    case AV_OPT_TYPE_INT64:     *intnum = *(int64_t     *)dst;return 0;
    case AV_OPT_TYPE_FLOAT:     *num    = *(float       *)dst;return 0;
    case AV_OPT_TYPE_DOUBLE:    *num    = *(double      *)dst;return 0;
    case AV_OPT_TYPE_RATIONAL:  *intnum = ((AVRational*)dst)->num;
                                *den    = ((AVRational*)dst)->den;
                                                        return 0;
    case AV_OPT_TYPE_CONST:     *num    = o->default_val.dbl; return 0;
    }
    return AVERROR(EINVAL);
}
