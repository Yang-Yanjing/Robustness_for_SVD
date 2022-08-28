static inline void compute_dar(AVRational *dar, AVRational sar, int w, int h)
{
    if (sar.num && sar.den) {
        av_reduce(&dar->num, &dar->den, sar.num * w, sar.den * h, INT_MAX);
    } else {
        av_reduce(&dar->num, &dar->den, w, h, INT_MAX);
    }
}
