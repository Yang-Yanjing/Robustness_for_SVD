static inline void rgb24tobgr24_c(const uint8_t *src, uint8_t *dst, int src_size)
{
    unsigned i;
    for (i = 0; i < src_size; i += 3) {
        register uint8_t x = src[i + 2];
        dst[i + 1]         = src[i + 1];
        dst[i + 2]         = src[i + 0];
        dst[i + 0]         = x;
    }
}
