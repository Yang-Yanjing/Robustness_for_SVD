static void vector_fmul_c(int *dst, const int *src0, const int *src1, int len)
{
    int i;
    int64_t accu;
    for (i = 0; i < len; i++){
        accu = (int64_t)src0[i] * src1[i];
        dst[i] = (int)((accu+0x40000000) >> 31);
    }
}
