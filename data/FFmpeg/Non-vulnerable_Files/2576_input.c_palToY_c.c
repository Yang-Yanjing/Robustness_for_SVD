static void palToY_c(uint8_t *_dst, const uint8_t *src, const uint8_t *unused1, const uint8_t *unused2, int width, uint32_t *pal)
{
    int16_t *dst = (int16_t *)_dst;
    int i;
    for (i = 0; i < width; i++) {
        int d = src[i];
        dst[i] = (pal[d] & 0xFF)<<6;
    }
}
