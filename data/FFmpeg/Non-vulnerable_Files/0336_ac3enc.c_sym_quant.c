static inline int sym_quant(int c, int e, int levels)
{
    int v = (((levels * c) >> (24 - e)) + levels) >> 1;
    av_assert2(v >= 0 && v < levels);
    return v;
}
