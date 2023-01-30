static void read_ayuv64le_UV_c(uint8_t *dstU, uint8_t *dstV, const uint8_t *unused0, const uint8_t *src,
                               const uint8_t *unused1, int width, uint32_t *unused2)
{
    int i;
    for (i = 0; i < width; i++) {
        AV_WN16(dstU + i * 2, AV_RL16(src + i * 8 + 4));
        AV_WN16(dstV + i * 2, AV_RL16(src + i * 8 + 6));
    }
}
