static inline void rgb24to16_c(const uint8_t *src, uint8_t *dst, int src_size)
{
    uint16_t *d        = (uint16_t *)dst;
    const uint8_t *s   = src;
    const uint8_t *end = s + src_size;
    while (s < end) {
        const int r = *s++;
        const int g = *s++;
        const int b = *s++;
        *d++        = (b >> 3) | ((g & 0xFC) << 3) | ((r & 0xF8) << 8);
    }
}
