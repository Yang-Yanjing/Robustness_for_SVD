static void uyvyToY_c(uint8_t *dst, const uint8_t *src, const uint8_t *unused1, const uint8_t *unused2,  int width,
                      uint32_t *unused)
{
    int i;
    for (i = 0; i < width; i++)
        dst[i] = src[2 * i + 1];
}