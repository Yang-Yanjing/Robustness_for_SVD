static void read_ya16le_alpha_c(uint8_t *dst, const uint8_t *src, const uint8_t *unused1, const uint8_t *unused2, int width,
                                uint32_t *unused)
{
    int i;
    for (i = 0; i < width; i++)
        AV_WN16(dst + i * 2, AV_RL16(src + i * 4 + 2));
}
