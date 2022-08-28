static void clip_coefficients(AudioDSPContext *adsp, float *coef,
                              unsigned int len)
{
    adsp->vector_clipf(coef, coef, COEF_MIN, COEF_MAX, len);
}
