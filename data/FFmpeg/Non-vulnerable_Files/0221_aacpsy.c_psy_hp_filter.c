static void psy_hp_filter(const float *firbuf, float *hpfsmpl, const float *psy_fir_coeffs)
{
    int i, j;
    for (i = 0; i < AAC_BLOCK_SIZE_LONG; i++) {
        float sum1, sum2;
        sum1 = firbuf[i + (PSY_LAME_FIR_LEN - 1) / 2];
        sum2 = 0.0;
        for (j = 0; j < ((PSY_LAME_FIR_LEN - 1) / 2) - 1; j += 2) {
            sum1 += psy_fir_coeffs[j] * (firbuf[i + j] + firbuf[i + PSY_LAME_FIR_LEN - j]);
            sum2 += psy_fir_coeffs[j + 1] * (firbuf[i + j + 1] + firbuf[i + PSY_LAME_FIR_LEN - j - 1]);
        }
        

        hpfsmpl[i] = (sum1 + sum2) * 32768.0f;
    }
}
