static av_always_inline SoftFloat flt16_trunc(SoftFloat pf)
{
    SoftFloat pun;
    int s;
    pun.exp = pf.exp;
    s = pf.mant >> 31;
    pun.mant = (pf.mant ^ s) - s;
    pun.mant = pun.mant & 0xFFC00000U;
    pun.mant = (pun.mant ^ s) - s;
    return pun;
}
