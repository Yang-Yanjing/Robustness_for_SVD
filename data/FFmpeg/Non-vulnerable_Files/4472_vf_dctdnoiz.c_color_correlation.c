static av_always_inline void color_correlation(uint8_t *dst, int dst_linesize,
                                               float **src, int src_linesize,
                                               int w, int h,
                                               int r, int g, int b)
{
    int x, y;
    const float *src_r = src[0];
    const float *src_g = src[1];
    const float *src_b = src[2];
    for (y = 0; y < h; y++) {
        uint8_t *dstp = dst;
        for (x = 0; x < w; x++) {
            dstp[r] = av_clip_uint8(src_r[x] * DCT3X3_0_0 + src_g[x] * DCT3X3_1_0 + src_b[x] * DCT3X3_2_0);
            dstp[g] = av_clip_uint8(src_r[x] * DCT3X3_0_1 +                         src_b[x] * DCT3X3_2_1);
            dstp[b] = av_clip_uint8(src_r[x] * DCT3X3_0_2 + src_g[x] * DCT3X3_1_2 + src_b[x] * DCT3X3_2_2);
            dstp += 3;
        }
        dst += dst_linesize;
        src_r += src_linesize;
        src_g += src_linesize;
        src_b += src_linesize;
    }
}
