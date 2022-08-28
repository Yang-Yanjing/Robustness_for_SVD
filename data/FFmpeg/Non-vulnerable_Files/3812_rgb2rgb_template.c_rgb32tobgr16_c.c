static inline void rgb32tobgr16_c(const uint8_t *src, uint8_t *dst,
                                  int src_size)
{
    uint16_t *d        = (uint16_t *)dst;
    const uint8_t *s   = src;
    const uint8_t *end = s + src_size;
    while (s < end) {
        register int rgb  = *(const uint32_t *)s;
        s                += 4;
        *d++              = ((rgb & 0xF8)     << 8) +
                            ((rgb & 0xFC00)   >> 5) +
                            ((rgb & 0xF80000) >> 19);
    }
}
