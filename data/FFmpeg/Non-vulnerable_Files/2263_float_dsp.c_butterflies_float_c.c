static void butterflies_float_c(float *av_restrict v1, float *av_restrict v2,
                                int len)
{
    int i;
    for (i = 0; i < len; i++) {
        float t = v1[i] - v2[i];
        v1[i] += v2[i];
        v2[i] = t;
    }
}
