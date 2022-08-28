static inline void rgb32tobgr15_c(const uint8_t *src, uint8_t *dst,
                                  int src_size)
{
    uint16_t *d        = (uint16_t *)dst;
    const uint8_t *s   = src;
    const uint8_t *end = s + src_size;
    while (s < end) {
        register int rgb  = *(const uint32_t *)s;
        s                += 4;
        *d++              = ((rgb & 0xF8)     <<  7) +
                            ((rgb & 0xF800)   >>  6) +
                            ((rgb & 0xF80000) >> 19);
    }
}
