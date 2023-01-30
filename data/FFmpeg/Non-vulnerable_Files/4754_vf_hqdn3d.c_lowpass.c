av_always_inline
static uint32_t lowpass(int prev, int cur, int16_t *coef, int depth)
{
    int d = (prev - cur) >> (8 - LUT_BITS);
    return cur + coef[d];
}
