static av_always_inline uint32_t interp_2px(uint32_t c1, int w1, uint32_t c2, int w2, int s)
{
    return (((((c1 & 0xff00ff00) >> 8) * w1 + ((c2 & 0xff00ff00) >> 8) * w2) << (8 - s)) & 0xff00ff00) |
           (((((c1 & 0x00ff00ff)     ) * w1 + ((c2 & 0x00ff00ff)     ) * w2) >>      s ) & 0x00ff00ff);
}
