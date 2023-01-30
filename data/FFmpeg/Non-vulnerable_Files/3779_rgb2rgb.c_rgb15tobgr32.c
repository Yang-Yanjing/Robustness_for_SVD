void rgb15tobgr32(const uint8_t *src, uint8_t *dst, int src_size)
{
    uint8_t *d          = dst;
    const uint16_t *s   = (const uint16_t *)src;
    const uint16_t *end = s + src_size / 2;
    while (s < end) {
        register uint16_t bgr = *s++;
#if HAVE_BIGENDIAN
        *d++ = 255;
        *d++ = ((bgr&0x001F)<<3) | ((bgr&0x001F)>> 2);
        *d++ = ((bgr&0x03E0)>>2) | ((bgr&0x03E0)>> 7);
        *d++ = ((bgr&0x7C00)>>7) | ((bgr&0x7C00)>>12);
#else
        *d++ = ((bgr&0x7C00)>>7) | ((bgr&0x7C00)>>12);
        *d++ = ((bgr&0x03E0)>>2) | ((bgr&0x03E0)>> 7);
        *d++ = ((bgr&0x001F)<<3) | ((bgr&0x001F)>> 2);
        *d++ = 255;
#endif
    }
}
