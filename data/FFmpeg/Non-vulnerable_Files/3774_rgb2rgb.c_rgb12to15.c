void rgb12to15(const uint8_t *src, uint8_t *dst, int src_size)
{
    uint16_t rgb, r, g, b;
    uint16_t *d         = (uint16_t *)dst;
    const uint16_t *s   = (const uint16_t *)src;
    const uint16_t *end = s + src_size / 2;
    while (s < end) {
        rgb  = *s++;
        r    = rgb & 0xF00;
        g    = rgb & 0x0F0;
        b    = rgb & 0x00F;
        r    = (r << 3) | ((r & 0x800) >> 1);
        g    = (g << 2) | ((g & 0x080) >> 2);
        b    = (b << 1) | ( b          >> 3);
        *d++ = r | g | b;
    }
}
