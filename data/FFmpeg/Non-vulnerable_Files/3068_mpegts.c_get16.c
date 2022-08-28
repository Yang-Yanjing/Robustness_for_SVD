static inline int get16(const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p;
    int c;
    p = *pp;
    if (1 >= p_end - p)
        return AVERROR_INVALIDDATA;
    c   = AV_RB16(p);
    p  += 2;
    *pp = p;
    return c;
}
