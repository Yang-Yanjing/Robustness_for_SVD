static void vector_pow43(int *coefs, int len)
{
    int i, coef;
    for (i=0; i<len; i++) {
        coef = coefs[i];
        if (coef < 0)
            coef = -(int)cbrt_tab[-coef];
        else
            coef = (int)cbrt_tab[coef];
        coefs[i] = coef;
    }
}
