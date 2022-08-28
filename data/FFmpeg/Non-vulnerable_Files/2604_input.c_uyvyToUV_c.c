static void uyvyToUV_c(uint8_t *dstU, uint8_t *dstV, const uint8_t *unused0, const uint8_t *src1,
                       const uint8_t *src2, int width, uint32_t *unused)
{
    int i;
    for (i = 0; i < width; i++) {
        dstU[i] = src1[4 * i + 0];
        dstV[i] = src1[4 * i + 2];
    }
    av_assert1(src1 == src2);
}
