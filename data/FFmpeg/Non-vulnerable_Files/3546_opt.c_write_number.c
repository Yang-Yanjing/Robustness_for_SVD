static int write_number(void *obj, const AVOption *o, void *dst, double num, int den, int64_t intnum)
{
    if (o->type != AV_OPT_TYPE_FLAGS &&
        (o->max * den < num * intnum || o->min * den > num * intnum)) {
        num = den ? num*intnum/den : (num*intnum ? INFINITY : NAN);
        av_log(obj, AV_LOG_ERROR, "Value %f for parameter '%s' out of range [%g - %g]\n",
               num, o->name, o->min, o->max);
        return AVERROR(ERANGE);
    }
    if (o->type == AV_OPT_TYPE_FLAGS) {
        double d = num*intnum/den;
        if (d < -1.5 || d > 0xFFFFFFFF+0.5 || (llrint(d*256) & 255)) {
            av_log(obj, AV_LOG_ERROR,
                   "Value %f for parameter '%s' is not a valid set of 32bit integer flags\n",
                   num*intnum/den, o->name);
            return AVERROR(ERANGE);
        }
    }
    switch (o->type) {
    case AV_OPT_TYPE_PIXEL_FMT: *(enum AVPixelFormat *)dst = llrint(num/den) * intnum; break;
    case AV_OPT_TYPE_SAMPLE_FMT:*(enum AVSampleFormat*)dst = llrint(num/den) * intnum; break;
    case AV_OPT_TYPE_FLAGS:
    case AV_OPT_TYPE_INT:   *(int       *)dst= llrint(num/den)*intnum; break;
    case AV_OPT_TYPE_DURATION:
    case AV_OPT_TYPE_CHANNEL_LAYOUT:
    case AV_OPT_TYPE_INT64: *(int64_t   *)dst= llrint(num/den)*intnum; break;
    case AV_OPT_TYPE_FLOAT: *(float     *)dst= num*intnum/den;         break;
    case AV_OPT_TYPE_DOUBLE:*(double    *)dst= num*intnum/den;         break;
    case AV_OPT_TYPE_RATIONAL:
        if ((int)num == num) *(AVRational*)dst= (AVRational){num*intnum, den};
        else                 *(AVRational*)dst= av_d2q(num*intnum/den, 1<<24);
        break;
    default:
        return AVERROR(EINVAL);
    }
    return 0;
}
