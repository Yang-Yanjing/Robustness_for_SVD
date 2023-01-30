static av_always_inline void
yuv2422_X_c_template(SwsContext *c, const int16_t *lumFilter,
                     const int16_t **lumSrc, int lumFilterSize,
                     const int16_t *chrFilter, const int16_t **chrUSrc,
                     const int16_t **chrVSrc, int chrFilterSize,
                     const int16_t **alpSrc, uint8_t *dest, int dstW,
                     int y, enum AVPixelFormat target)
{
    int i;
    for (i = 0; i < ((dstW + 1) >> 1); i++) {
        int j;
        int Y1 = 1 << 18;
        int Y2 = 1 << 18;
        int U  = 1 << 18;
        int V  = 1 << 18;
        for (j = 0; j < lumFilterSize; j++) {
            Y1 += lumSrc[j][i * 2]     * lumFilter[j];
            Y2 += lumSrc[j][i * 2 + 1] * lumFilter[j];
        }
        for (j = 0; j < chrFilterSize; j++) {
            U += chrUSrc[j][i] * chrFilter[j];
            V += chrVSrc[j][i] * chrFilter[j];
        }
        Y1 >>= 19;
        Y2 >>= 19;
        U  >>= 19;
        V  >>= 19;
        if ((Y1 | Y2 | U | V) & 0x100) {
            Y1 = av_clip_uint8(Y1);
            Y2 = av_clip_uint8(Y2);
            U  = av_clip_uint8(U);
            V  = av_clip_uint8(V);
        }
        output_pixels(4*i, Y1, U, Y2, V);
    }
}
