static inline void rgb24tobgr32_c(const uint8_t *src, uint8_t *dst,
                                  int src_size)
{
    uint8_t *dest      = dst;
    const uint8_t *s   = src;
    const uint8_t *end = s + src_size;
    while (s < end) {
#if HAVE_BIGENDIAN
        
        *dest++  = 255;
        *dest++  = s[2];
        *dest++  = s[1];
        *dest++  = s[0];
        s       += 3;
#else
        *dest++  = *s++;
        *dest++  = *s++;
        *dest++  = *s++;
        *dest++  = 255;
#endif
    }
}
