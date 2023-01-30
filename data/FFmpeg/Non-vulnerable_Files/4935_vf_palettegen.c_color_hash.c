static inline unsigned color_hash(uint32_t color)
{
    const uint8_t r = color >> 16 & ((1<<NBITS)-1);
    const uint8_t g = color >>  8 & ((1<<NBITS)-1);
    const uint8_t b = color       & ((1<<NBITS)-1);
    return r<<(NBITS*2) | g<<NBITS | b;
}
