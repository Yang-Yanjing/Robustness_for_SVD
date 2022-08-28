void rgb16tobgr15(const uint8_t *src, uint8_t *dst, int src_size)
{
    int i, num_pixels = src_size >> 1;
    for (i = 0; i < num_pixels; i++) {
        unsigned rgb         = ((const uint16_t *)src)[i];
        ((uint16_t *)dst)[i] = (rgb >> 11) | ((rgb & 0x7C0) >> 1) | ((rgb & 0x1F) << 10);
    }
}
