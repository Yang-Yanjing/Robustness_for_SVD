AVFILTER_DEFINE_CLASS(hqx);
static av_always_inline uint32_t rgb2yuv(const uint32_t *r2y, uint32_t c)
{
    return r2y[c & 0xffffff];
}
