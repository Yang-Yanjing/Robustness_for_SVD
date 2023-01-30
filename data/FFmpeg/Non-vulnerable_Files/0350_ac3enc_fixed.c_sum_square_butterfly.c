static void sum_square_butterfly(AC3EncodeContext *s, int64_t sum[4],
                                 const int32_t *coef0, const int32_t *coef1,
                                 int len)
{
    s->ac3dsp.sum_square_butterfly_int32(sum, coef0, coef1, len);
}
