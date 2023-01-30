static av_always_inline void rgb16_32ToY_c_template(int16_t *dst,
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
    const int ry       = rgb2yuv[RY_IDX]<<rsh, gy = rgb2yuv[GY_IDX]<<gsh, by = rgb2yuv[BY_IDX]<<bsh;
    const unsigned rnd = (32<<((S)-1)) + (1<<(S-7));
    int i;

    for (i = 0; i < width; i++) {
        int px = input_pixel(i) >> shp;
        int b  = (px & maskb) >> shb;
        int g  = (px & maskg) >> shg;
        int r  = (px & maskr) >> shr;

        dst[i] = (ry * r + gy * g + by * b + rnd) >> ((S)-6);
    }
}