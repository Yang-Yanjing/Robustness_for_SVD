static int int_cos(int a)
{
    int v, neg;
    a = a & (FRAC_ONE - 1);
    if (a >= (FRAC_ONE / 2))
        a = FRAC_ONE - a;
    neg = 0;
    if (a > (FRAC_ONE / 4)) {
        neg = -1;
        a   = (FRAC_ONE / 2) - a;
    }
    v = FRAC_ONE - ((a * a) >> 4);
    v = (v ^ neg) - neg;
    return v;
}
