static av_always_inline SoftFloat flt16_round(SoftFloat pf)
{
    SoftFloat tmp;
    int s;
    tmp.exp = pf.exp;
    s = pf.mant >> 31;
    tmp.mant = (pf.mant ^ s) - s;
    tmp.mant = (tmp.mant + 0x00200000U) & 0xFFC00000U;
    tmp.mant = (tmp.mant ^ s) - s;
    return tmp;
}
