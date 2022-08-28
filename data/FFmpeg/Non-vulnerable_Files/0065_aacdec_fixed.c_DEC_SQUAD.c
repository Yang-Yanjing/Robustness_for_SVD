static inline int *DEC_SQUAD(int *dst, unsigned idx)
{
    dst[0] = (idx & 3) - 1;
    dst[1] = (idx >> 2 & 3) - 1;
    dst[2] = (idx >> 4 & 3) - 1;
    dst[3] = (idx >> 6 & 3) - 1;
    return dst + 4;
}
