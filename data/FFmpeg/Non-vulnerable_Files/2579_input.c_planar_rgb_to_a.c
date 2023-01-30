static void planar_rgb_to_a(uint8_t *_dst, const uint8_t *src[4], int width, int32_t *unused)
{
    uint16_t *dst = (uint16_t *)_dst;
    int i;
    for (i = 0; i < width; i++)
        dst[i] = src[3][i] << 6;
}
