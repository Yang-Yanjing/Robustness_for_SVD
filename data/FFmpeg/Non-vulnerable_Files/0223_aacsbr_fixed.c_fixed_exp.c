static int fixed_exp(int x)
{
    int i, ret, xpow, tmp;
    ret = 0x800000 + x;
    xpow = x;
    for (i=0; i<7; i++){
        xpow = (int)(((int64_t)xpow * x + 0x400000) >> 23);
        tmp = (int)(((int64_t)xpow * fixed_exp_table[i] + 0x40000000) >> 31);
        ret += tmp;
    }
    return ret;
}
