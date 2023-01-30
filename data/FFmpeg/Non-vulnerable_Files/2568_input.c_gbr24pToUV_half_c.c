rgb16_32_wrapper(AV_PIX_FMT_RGB444BE, rgb12be, 0, 0,  0, 0,   0x0F00, 0x00F0,   0x000F,  0, 4,  8, RGB2YUV_SHIFT + 4)
static void gbr24pToUV_half_c(uint8_t *_dstU, uint8_t *_dstV,
                         const uint8_t *gsrc, const uint8_t *bsrc, const uint8_t *rsrc,
                         int width, uint32_t *rgb2yuv)
{
    uint16_t *dstU = (uint16_t *)_dstU;
    uint16_t *dstV = (uint16_t *)_dstV;
    int32_t ru = rgb2yuv[RU_IDX], gu = rgb2yuv[GU_IDX], bu = rgb2yuv[BU_IDX];
    int32_t rv = rgb2yuv[RV_IDX], gv = rgb2yuv[GV_IDX], bv = rgb2yuv[BV_IDX];
    int i;
    for (i = 0; i < width; i++) {
        unsigned int g   = gsrc[2*i] + gsrc[2*i+1];
        unsigned int b   = bsrc[2*i] + bsrc[2*i+1];
        unsigned int r   = rsrc[2*i] + rsrc[2*i+1];
        dstU[i] = (ru*r + gu*g + bu*b + (0x4001<<(RGB2YUV_SHIFT-6))) >> (RGB2YUV_SHIFT-6+1);
        dstV[i] = (rv*r + gv*g + bv*b + (0x4001<<(RGB2YUV_SHIFT-6))) >> (RGB2YUV_SHIFT-6+1);
    }
}
