static void mix_1_to_2_fltp_flt_c(float **samples, float **matrix, int len,
                                  int out_ch, int in_ch)
{
    float v;
    float *dst0 = samples[0];
    float *dst1 = samples[1];
    float *src  = dst0;
    float m0    = matrix[0][0];
    float m1    = matrix[1][0];
    while (len > 4) {
        v = *src++;
        *dst0++ = v * m0;
        *dst1++ = v * m1;
        v = *src++;
        *dst0++ = v * m0;
        *dst1++ = v * m1;
        v = *src++;
        *dst0++ = v * m0;
        *dst1++ = v * m1;
        v = *src++;
        *dst0++ = v * m0;
        *dst1++ = v * m1;
        len -= 4;
    }
    while (len > 0) {
        v = *src++;
        *dst0++ = v * m0;
        *dst1++ = v * m1;
        len--;
    }
}
