static void gamma_convert(uint8_t * src[], int width, uint16_t *gamma)
{
    int i;
    uint16_t *src1 = (uint16_t*)src[0];
    for (i = 0; i < width; ++i) {
        uint16_t r = AV_RL16(src1 + i*4 + 0);
        uint16_t g = AV_RL16(src1 + i*4 + 1);
        uint16_t b = AV_RL16(src1 + i*4 + 2);
        AV_WL16(src1 + i*4 + 0, gamma[r]);
        AV_WL16(src1 + i*4 + 1, gamma[g]);
        AV_WL16(src1 + i*4 + 2, gamma[b]);
    }
}
