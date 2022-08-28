static void mix_2_to_1_fltp_flt_c(float **samples, float **matrix, int len,
                                  int out_ch, int in_ch)
{
    float *src0 = samples[0];
    float *src1 = samples[1];
    float *dst  = src0;
    float m0    = matrix[0][0];
    float m1    = matrix[0][1];
    while (len > 4) {
        *dst++ = *src0++ * m0 + *src1++ * m1;
        *dst++ = *src0++ * m0 + *src1++ * m1;
        *dst++ = *src0++ * m0 + *src1++ * m1;
        *dst++ = *src0++ * m0 + *src1++ * m1;
        len -= 4;
    }
    while (len > 0) {
        *dst++ = *src0++ * m0 + *src1++ * m1;
        len--;
    }
}
