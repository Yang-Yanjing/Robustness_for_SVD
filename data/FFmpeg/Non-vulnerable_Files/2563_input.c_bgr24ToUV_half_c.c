static void bgr24ToUV_half_c(uint8_t *_dstU, uint8_t *_dstV, const uint8_t *unused0, const uint8_t *src1,
                             const uint8_t *src2, int width, uint32_t *rgb2yuv)
{
    int16_t *dstU = (int16_t *)_dstU;
    int16_t *dstV = (int16_t *)_dstV;
    int i;
    int32_t ru = rgb2yuv[RU_IDX], gu = rgb2yuv[GU_IDX], bu = rgb2yuv[BU_IDX];
    int32_t rv = rgb2yuv[RV_IDX], gv = rgb2yuv[GV_IDX], bv = rgb2yuv[BV_IDX];
    for (i = 0; i < width; i++) {
        int b = src1[6 * i + 0] + src1[6 * i + 3];
        int g = src1[6 * i + 1] + src1[6 * i + 4];
        int r = src1[6 * i + 2] + src1[6 * i + 5];
        dstU[i] = (ru*r + gu*g + bu*b + (256<<RGB2YUV_SHIFT) + (1<<(RGB2YUV_SHIFT-6)))>>(RGB2YUV_SHIFT-5);
        dstV[i] = (rv*r + gv*g + bv*b + (256<<RGB2YUV_SHIFT) + (1<<(RGB2YUV_SHIFT-6)))>>(RGB2YUV_SHIFT-5);
    }
    av_assert1(src1 == src2);
}
