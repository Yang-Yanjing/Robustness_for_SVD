void avfilter_add_matrix(const float *m1, const float *m2, float *result)
{
    int i;
    for (i = 0; i < 9; i++)
        result[i] = m1[i] + m2[i];
}
