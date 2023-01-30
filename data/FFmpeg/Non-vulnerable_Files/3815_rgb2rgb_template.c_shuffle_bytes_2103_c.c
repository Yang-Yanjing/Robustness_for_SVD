static inline void shuffle_bytes_2103_c(const uint8_t *src, uint8_t *dst,
                                        int src_size)
{
    int idx          = 15  - src_size;
    const uint8_t *s = src - idx;
    uint8_t *d       = dst - idx;
    for (; idx < 15; idx += 4) {
        register unsigned v   = *(const uint32_t *)&s[idx], g = v & 0xff00ff00;
        v                    &= 0xff00ff;
        *(uint32_t *)&d[idx]  = (v >> 16) + g + (v << 16);
    }
}
