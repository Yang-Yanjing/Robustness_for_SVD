static inline void rgb16to15_c(const uint8_t *src, uint8_t *dst, int src_size)
{
    register uint8_t *d         = dst;
    register const uint8_t *s   = src;
    register const uint8_t *end = s + src_size;
    const uint8_t *mm_end       = end - 3;
    while (s < mm_end) {
        register uint32_t x  = *((const uint32_t *)s);
        *((uint32_t *)d)     = ((x >> 1) & 0x7FE07FE0) | (x & 0x001F001F);
        s                   += 4;
        d                   += 4;
    }
    if (s < end) {
        register uint16_t x = *((const uint16_t *)s);
        *((uint16_t *)d)    = ((x >> 1) & 0x7FE0) | (x & 0x001F);
    }
}
