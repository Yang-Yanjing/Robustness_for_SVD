static void ac3_extract_exponents_c(uint8_t *exp, int32_t *coef, int nb_coefs)
{
    int i;
    for (i = 0; i < nb_coefs; i++) {
        int v = abs(coef[i]);
        exp[i] = v ? 23 - av_log2(v) : 24;
    }
}
