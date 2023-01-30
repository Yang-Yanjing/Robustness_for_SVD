static void bswap16Y_c(uint8_t *_dst, const uint8_t *_src, const uint8_t *unused1, const uint8_t *unused2, int width,
                       uint32_t *unused)
{
    int i;
    const uint16_t *src = (const uint16_t *)_src;
    uint16_t *dst       = (uint16_t *)_dst;
    for (i = 0; i < width; i++)
        dst[i] = av_bswap16(src[i]);
}
