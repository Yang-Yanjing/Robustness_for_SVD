static void rgb24ToY_c(uint8_t *_dst, const uint8_t *src, const uint8_t *unused1, const uint8_t *unused2, int width,
                       uint32_t *rgb2yuv)
{
    int16_t *dst = (int16_t *)_dst;
    int32_t ry = rgb2yuv[RY_IDX], gy = rgb2yuv[GY_IDX], by = rgb2yuv[BY_IDX];
    int i;
    for (i = 0; i < width; i++) {
        int r = src[i * 3 + 0];
        int g = src[i * 3 + 1];
        int b = src[i * 3 + 2];
        dst[i] = ((ry*r + gy*g + by*b + (32<<(RGB2YUV_SHIFT-1)) + (1<<(RGB2YUV_SHIFT-7)))>>(RGB2YUV_SHIFT-6));
    }
}
