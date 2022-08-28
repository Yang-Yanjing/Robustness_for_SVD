static void planar_rgb_to_y(uint8_t *_dst, const uint8_t *src[4], int width, int32_t *rgb2yuv)
{
    uint16_t *dst = (uint16_t *)_dst;
    int32_t ry = rgb2yuv[RY_IDX], gy = rgb2yuv[GY_IDX], by = rgb2yuv[BY_IDX];
    int i;
    for (i = 0; i < width; i++) {
        int g = src[0][i];
        int b = src[1][i];
        int r = src[2][i];
        dst[i] = (ry*r + gy*g + by*b + (0x801<<(RGB2YUV_SHIFT-7))) >> (RGB2YUV_SHIFT-6);
    }
}
