static void bswap16UV_c(uint8_t *_dstU, uint8_t *_dstV, const uint8_t *unused0, const uint8_t *_src1,
                        const uint8_t *_src2, int width, uint32_t *unused)
{
    int i;
    const uint16_t *src1 = (const uint16_t *)_src1,
    *src2                = (const uint16_t *)_src2;
    uint16_t *dstU       = (uint16_t *)_dstU, *dstV = (uint16_t *)_dstV;
    for (i = 0; i < width; i++) {
        dstU[i] = av_bswap16(src1[i]);
        dstV[i] = av_bswap16(src2[i]);
    }
}
