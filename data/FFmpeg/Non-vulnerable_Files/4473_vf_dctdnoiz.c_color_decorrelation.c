static av_always_inline void color_decorrelation(float **dst, int dst_linesize,
                                                 const uint8_t *src, int src_linesize,
                                                 int w, int h,
                                                 int r, int g, int b)
{
    int x, y;
    float *dstp_r = dst[0];
    float *dstp_g = dst[1];
    float *dstp_b = dst[2];
    for (y = 0; y < h; y++) {
        const uint8_t *srcp = src;
        for (x = 0; x < w; x++) {
            dstp_r[x] = srcp[r] * DCT3X3_0_0 + srcp[g] * DCT3X3_0_1 + srcp[b] * DCT3X3_0_2;
            dstp_g[x] = srcp[r] * DCT3X3_1_0 +                        srcp[b] * DCT3X3_1_2;
            dstp_b[x] = srcp[r] * DCT3X3_2_0 + srcp[g] * DCT3X3_2_1 + srcp[b] * DCT3X3_2_2;
            srcp += 3;
        }
        src += src_linesize;
        dstp_r += dst_linesize;
        dstp_g += dst_linesize;
        dstp_b += dst_linesize;
    }
}
