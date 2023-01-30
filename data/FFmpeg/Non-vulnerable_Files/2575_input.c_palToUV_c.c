static void palToUV_c(uint8_t *_dstU, uint8_t *_dstV,
                           const uint8_t *unused0, const uint8_t *src1, const uint8_t *src2,
                      int width, uint32_t *pal)
{
    uint16_t *dstU = (uint16_t *)_dstU;
    int16_t *dstV = (int16_t *)_dstV;
    int i;
    av_assert1(src1 == src2);
    for (i = 0; i < width; i++) {
        int p = pal[src1[i]];
        dstU[i] = (uint8_t)(p>> 8)<<6;
        dstV[i] = (uint8_t)(p>>16)<<6;
    }
}
