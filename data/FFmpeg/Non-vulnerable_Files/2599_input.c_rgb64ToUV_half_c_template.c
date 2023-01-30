static av_always_inline void
rgb64ToUV_half_c_template(uint16_t *dstU, uint16_t *dstV,
                          const uint16_t *src1, const uint16_t *src2,
                          int width, enum AVPixelFormat origin, int32_t *rgb2yuv)
{
    int i;
    int32_t ru = rgb2yuv[RU_IDX], gu = rgb2yuv[GU_IDX], bu = rgb2yuv[BU_IDX];
    int32_t rv = rgb2yuv[RV_IDX], gv = rgb2yuv[GV_IDX], bv = rgb2yuv[BV_IDX];
    av_assert1(src1==src2);
    for (i = 0; i < width; i++) {
        int r_b = (input_pixel(&src1[8 * i + 0]) + input_pixel(&src1[8 * i + 4]) + 1) >> 1;
        int   g = (input_pixel(&src1[8 * i + 1]) + input_pixel(&src1[8 * i + 5]) + 1) >> 1;
        int b_r = (input_pixel(&src1[8 * i + 2]) + input_pixel(&src1[8 * i + 6]) + 1) >> 1;
        dstU[i]= (ru*r + gu*g + bu*b + (0x10001<<(RGB2YUV_SHIFT-1))) >> RGB2YUV_SHIFT;
        dstV[i]= (rv*r + gv*g + bv*b + (0x10001<<(RGB2YUV_SHIFT-1))) >> RGB2YUV_SHIFT;
    }
}
