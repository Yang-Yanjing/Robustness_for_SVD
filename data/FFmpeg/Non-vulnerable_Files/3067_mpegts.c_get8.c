static inline int get8(const uint8_t **pp, const uint8_t *p_end)
{
    const uint8_t *p;
    int c;
    p = *pp;
    if (p >= p_end)
        return AVERROR_INVALIDDATA;
    c   = *p++;
    *pp = p;
    return c;
}
