static inline int *DEC_SPAIR(int *dst, unsigned idx)
{
    dst[0] = (idx & 15) - 4;
    dst[1] = (idx >> 4 & 15) - 4;
    return dst + 2;
}
