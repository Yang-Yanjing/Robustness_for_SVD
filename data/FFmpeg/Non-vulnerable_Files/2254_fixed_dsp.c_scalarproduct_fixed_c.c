static int scalarproduct_fixed_c(const int *v1, const int *v2, int len)
{
    

    int64_t p = 0x40000000;
    int i;
    for (i = 0; i < len; i++)
        p += (int64_t)v1[i] * v2[i];
    return (int)(p >> 31);
}
