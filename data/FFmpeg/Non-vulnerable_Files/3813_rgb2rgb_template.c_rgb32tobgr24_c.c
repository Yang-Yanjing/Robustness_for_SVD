static inline void rgb32tobgr24_c(const uint8_t *src, uint8_t *dst,
                                  int src_size)
{
    uint8_t *dest      = dst;
    const uint8_t *s   = src;
    const uint8_t *end = s + src_size;
    while (s < end) {
#if HAVE_BIGENDIAN
        
        s++;
        dest[2]  = *s++;
        dest[1]  = *s++;
        dest[0]  = *s++;
        dest    += 3;
#else
        *dest++  = *s++;
        *dest++  = *s++;
        *dest++  = *s++;
        s++;
#endif
    }
}
