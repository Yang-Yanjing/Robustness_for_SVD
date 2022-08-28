static inline void blur(uint8_t *dst, int dst_step, const uint8_t *src, int src_step,
                        int len, int radius, int pixsize)
{
    if (pixsize == 1) blur8 (dst, dst_step   , src, src_step   , len, radius);
    else              blur16((uint16_t*)dst, dst_step>>1, (const uint16_t*)src, src_step>>1, len, radius);
}
