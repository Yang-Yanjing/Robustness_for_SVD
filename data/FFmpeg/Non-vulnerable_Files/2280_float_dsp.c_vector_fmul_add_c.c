static void vector_fmul_add_c(float *dst, const float *src0, const float *src1,
                              const float *src2, int len){
    int i;
    for (i = 0; i < len; i++)
        dst[i] = src0[i] * src1[i] + src2[i];
}
