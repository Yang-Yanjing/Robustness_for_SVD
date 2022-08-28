static void mix_2_to_1_s16p_q8_c(int16_t **samples, int16_t **matrix, int len,
                                 int out_ch, int in_ch)
{
    int16_t *src0 = samples[0];
    int16_t *src1 = samples[1];
    int16_t *dst  = src0;
    int16_t m0    = matrix[0][0];
    int16_t m1    = matrix[0][1];
    while (len > 4) {
        *dst++ = (*src0++ * m0 + *src1++ * m1) >> 8;
        *dst++ = (*src0++ * m0 + *src1++ * m1) >> 8;
        *dst++ = (*src0++ * m0 + *src1++ * m1) >> 8;
        *dst++ = (*src0++ * m0 + *src1++ * m1) >> 8;
        len -= 4;
    }
    while (len > 0) {
        *dst++ = (*src0++ * m0 + *src1++ * m1) >> 8;
        len--;
    }
}
