static void ps_mul_pair_single_c(INTFLOAT (*dst)[2], INTFLOAT (*src0)[2], INTFLOAT *src1,
                                 int n)
{
    int i;
    for (i = 0; i < n; i++) {
        dst[i][0] = AAC_MUL16(src0[i][0], src1[i]);
        dst[i][1] = AAC_MUL16(src0[i][1], src1[i]);
    }
}
