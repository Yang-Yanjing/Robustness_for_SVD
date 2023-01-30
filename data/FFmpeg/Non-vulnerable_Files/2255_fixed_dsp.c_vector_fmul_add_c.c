static void vector_fmul_add_c(int *dst, const int *src0, const int *src1, const int *src2, int len){
    int i;
    int64_t accu;
    for (i=0; i<len; i++) {
        accu = (int64_t)src0[i] * src1[i];
        dst[i] = src2[i] + (int)((accu + 0x40000000) >> 31);
    }
}
