static void vector_fmul_window_c(float *dst, const float *src0,
                                 const float *src1, const float *win, int len)
{
    int i, j;
    dst  += len;
    win  += len;
    src0 += len;
    for (i = -len, j = len - 1; i < 0; i++, j--) {
        float s0 = src0[i];
        float s1 = src1[j];
        float wi = win[i];
        float wj = win[j];
        dst[i] = s0 * wj - s1 * wi;
        dst[j] = s0 * wi + s1 * wj;
    }
}
