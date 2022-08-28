static av_always_inline void planar_rgb16_to_uv(uint8_t *_dstU, uint8_t *_dstV,
                                                const uint8_t *_src[4], int width,
                                                int bpc, int is_be, int32_t *rgb2yuv)
{
    int i;
    const uint16_t **src = (const uint16_t **)_src;
    uint16_t *dstU       = (uint16_t *)_dstU;
    uint16_t *dstV       = (uint16_t *)_dstV;
    int32_t ru = rgb2yuv[RU_IDX], gu = rgb2yuv[GU_IDX], bu = rgb2yuv[BU_IDX];
    int32_t rv = rgb2yuv[RV_IDX], gv = rgb2yuv[GV_IDX], bv = rgb2yuv[BV_IDX];
    int shift = bpc < 16 ? bpc : 14;
    for (i = 0; i < width; i++) {
        int g = rdpx(src[0] + i);
        int b = rdpx(src[1] + i);
        int r = rdpx(src[2] + i);
        dstU[i] = (ru*r + gu*g + bu*b + (257 << (RGB2YUV_SHIFT + bpc - 9))) >> (RGB2YUV_SHIFT + shift - 14);
        dstV[i] = (rv*r + gv*g + bv*b + (257 << (RGB2YUV_SHIFT + bpc - 9))) >> (RGB2YUV_SHIFT + shift - 14);
    }
}
