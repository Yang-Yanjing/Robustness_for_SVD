static inline void yv12toyuy2_c(const uint8_t *ysrc, const uint8_t *usrc,
                                const uint8_t *vsrc, uint8_t *dst,
                                int width, int height, int lumStride,
                                int chromStride, int dstStride)
{
    
    yuvPlanartoyuy2_c(ysrc, usrc, vsrc, dst, width, height, lumStride,
                      chromStride, dstStride, 2);
}
