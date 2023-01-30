static inline void rgb15to16_c(const uint8_t *src, uint8_t *dst, int src_size)
{
    register uint8_t *d         = dst;
    register const uint8_t *s   = src;
    register const uint8_t *end = s + src_size;
    const uint8_t *mm_end       = end - 3;
    while (s < mm_end) {
        register unsigned x = *((const uint32_t *)s);
        *((uint32_t *)d)    = (x & 0x7FFF7FFF) + (x & 0x7FE07FE0);
        d += 4;
        s += 4;
    }
    if (s < end) {
        register unsigned short x = *((const uint16_t *)s);
        *((uint16_t *)d)          = (x & 0x7FFF) + (x & 0x7FE0);
    }
}
