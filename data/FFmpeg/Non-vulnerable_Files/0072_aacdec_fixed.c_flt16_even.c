static av_always_inline SoftFloat flt16_even(SoftFloat pf)
{
    SoftFloat tmp;
    int s;
    tmp.exp = pf.exp;
    s = pf.mant >> 31;
    tmp.mant = (pf.mant ^ s) - s;
    tmp.mant = (tmp.mant + 0x001FFFFFU + (tmp.mant & 0x00400000U >> 16)) & 0xFFC00000U;
    tmp.mant = (tmp.mant ^ s) - s;
    return tmp;
}
