static void rgbaToA_c(uint8_t *_dst, const uint8_t *src, const uint8_t *unused1, const uint8_t *unused2, int width, uint32_t *unused)
{
    int16_t *dst = (int16_t *)_dst;
    int i;
    for (i=0; i<width; i++) {
        dst[i]= src[4*i+3]<<6;
    }
}
