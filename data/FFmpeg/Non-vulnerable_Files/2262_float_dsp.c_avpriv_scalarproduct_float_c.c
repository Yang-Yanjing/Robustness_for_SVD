float avpriv_scalarproduct_float_c(const float *v1, const float *v2, int len)
{
    float p = 0.0;
    int i;
    for (i = 0; i < len; i++)
        p += v1[i] * v2[i];
    return p;
}
