static inline float flt16_trunc(float pf)
{
    union av_intfloat32 pun;
    pun.f = pf;
    pun.i &= 0xFFFF0000U;
    return pun.f;
}
