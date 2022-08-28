static inline int
symmetric_dequant(int code, int levels)
{
    return ((code - (levels >> 1)) * (1 << 24)) / levels;
}
