static void sum_square_butterfly(AC3EncodeContext *s, float sum[4],
                                 const float *coef0, const float *coef1,
                                 int len)
{
    s->ac3dsp.sum_square_butterfly_float(sum, coef0, coef1, len);
}
