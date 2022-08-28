void avfilter_mul_matrix(const float *m1, float scalar, float *result)
{
    int i;
    for (i = 0; i < 9; i++)
        result[i] = m1[i] * scalar;
}
