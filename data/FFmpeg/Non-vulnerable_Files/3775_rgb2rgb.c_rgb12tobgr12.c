void rgb12tobgr12(const uint8_t *src, uint8_t *dst, int src_size)
{
    uint16_t *d = (uint16_t *)dst;
    uint16_t *s = (uint16_t *)src;
    int i, num_pixels = src_size >> 1;
    for (i = 0; i < num_pixels; i++) {
        unsigned rgb = s[i];
        d[i]         = (rgb << 8 | rgb & 0xF0 | rgb >> 8) & 0xFFF;
    }
}
