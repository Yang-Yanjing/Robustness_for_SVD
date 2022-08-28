static av_always_inline void rgb16_32ToUV_c_template(int16_t *dstU,
                                                     int16_t *dstV,
                                                     const uint8_t *src,
                                                     int width,
                                                     enum AVPixelFormat origin,
                                                     int shr, int shg,
                                                     int shb, int shp,
                                                     int maskr, int maskg,
                                                     int maskb, int rsh,
                                                     int gsh, int bsh, int S,
                                                     int32_t *rgb2yuv)
{
    const int ru       = rgb2yuv[RU_IDX] << rsh, gu = rgb2yuv[GU_IDX] << gsh, bu = rgb2yuv[BU_IDX] << bsh,
              rv       = rgb2yuv[RV_IDX] << rsh, gv = rgb2yuv[GV_IDX] << gsh, bv = rgb2yuv[BV_IDX] << bsh;
    const unsigned rnd = (256u<<((S)-1)) + (1<<(S-7));
    int i;

    for (i = 0; i < width; i++) {
        int px = input_pixel(i) >> shp;
        int b  = (px & maskb)   >> shb;
        int g  = (px & maskg)   >> shg;
        int r  = (px & maskr)   >> shr;

        dstU[i] = (ru * r + gu * g + bu * b + rnd) >> ((S)-6);
        dstV[i] = (rv * r + gv * g + bv * b + rnd) >> ((S)-6);
    }
}