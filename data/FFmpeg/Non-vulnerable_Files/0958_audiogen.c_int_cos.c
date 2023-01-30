static int int_cos(int a)
{
    int neg, v, f;
    const unsigned short *p;
    a = a & (FRAC_ONE - 1); 
    if (a >= (FRAC_ONE / 2))
        a = FRAC_ONE - a;
    neg = 0;
    if (a > (FRAC_ONE / 4)) {
        neg = -1;
        a   = (FRAC_ONE / 2) - a;
    }
    p = cos_table + (a >> CSHIFT);
    
    f = a & ((1 << CSHIFT) - 1);
    v = p[0] + (((p[1] - p[0]) * f + (1 << (CSHIFT - 1))) >> CSHIFT);
    v = (v ^ neg) - neg;
    v = v << (FRAC_BITS - 15);
    return v;
}
