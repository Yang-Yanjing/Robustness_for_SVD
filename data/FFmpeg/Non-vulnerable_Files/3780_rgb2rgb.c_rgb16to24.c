void rgb16to24(const uint8_t *src, uint8_t *dst, int src_size)
{
    uint8_t *d          = dst;
    const uint16_t *s   = (const uint16_t *)src;
    const uint16_t *end = s + src_size / 2;
    while (s < end) {
        register uint16_t bgr = *s++;
        *d++ = ((bgr&0xF800)>>8) | ((bgr&0xF800)>>13);
        *d++ = ((bgr&0x07E0)>>3) | ((bgr&0x07E0)>> 9);
        *d++ = ((bgr&0x001F)<<3) | ((bgr&0x001F)>> 2);
    }
}
