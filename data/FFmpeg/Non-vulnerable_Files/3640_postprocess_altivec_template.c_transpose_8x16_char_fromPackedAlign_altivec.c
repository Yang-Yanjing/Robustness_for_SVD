static inline void transpose_8x16_char_fromPackedAlign_altivec(unsigned char* dst, unsigned char* src, int stride) {
    const vector unsigned char zero = vec_splat_u8(0);
    const vector signed   char neg1 = vec_splat_s8(-1);
#define LOAD_DOUBLE_LINE(i, j)                                  \
    vector unsigned char src##i = vec_ld(i * 16, src);            \
    vector unsigned char src##j = vec_ld(j * 16, src)
    LOAD_DOUBLE_LINE(0, 1);
    LOAD_DOUBLE_LINE(2, 3);
    LOAD_DOUBLE_LINE(4, 5);
    LOAD_DOUBLE_LINE(6, 7);
    LOAD_DOUBLE_LINE(8, 9);
    LOAD_DOUBLE_LINE(10, 11);
    LOAD_DOUBLE_LINE(12, 13);
    LOAD_DOUBLE_LINE(14, 15);
#undef LOAD_DOUBLE_LINE
    vector unsigned char tempA = vec_mergeh(src0, src8);
    vector unsigned char tempB;
    vector unsigned char tempC = vec_mergeh(src1, src9);
    vector unsigned char tempD;
    vector unsigned char tempE = vec_mergeh(src2, src10);
    vector unsigned char tempG = vec_mergeh(src3, src11);
    vector unsigned char tempI = vec_mergeh(src4, src12);
    vector unsigned char tempJ;
    vector unsigned char tempK = vec_mergeh(src5, src13);
    vector unsigned char tempL;
    vector unsigned char tempM = vec_mergeh(src6, src14);
    vector unsigned char tempO = vec_mergeh(src7, src15);
    vector unsigned char temp0 = vec_mergeh(tempA, tempI);
    vector unsigned char temp1 = vec_mergel(tempA, tempI);
    vector unsigned char temp2;
    vector unsigned char temp3;
    vector unsigned char temp4 = vec_mergeh(tempC, tempK);
    vector unsigned char temp5 = vec_mergel(tempC, tempK);
    vector unsigned char temp6;
    vector unsigned char temp7;
    vector unsigned char temp8 = vec_mergeh(tempE, tempM);
    vector unsigned char temp9 = vec_mergel(tempE, tempM);
    vector unsigned char temp12 = vec_mergeh(tempG, tempO);
    vector unsigned char temp13 = vec_mergel(tempG, tempO);
    tempA = vec_mergeh(temp0, temp8);
    tempB = vec_mergel(temp0, temp8);
    tempC = vec_mergeh(temp1, temp9);
    tempD = vec_mergel(temp1, temp9);
    tempI = vec_mergeh(temp4, temp12);
    tempJ = vec_mergel(temp4, temp12);
    tempK = vec_mergeh(temp5, temp13);
    tempL = vec_mergel(temp5, temp13);
    temp0 = vec_mergeh(tempA, tempI);
    temp1 = vec_mergel(tempA, tempI);
    temp2 = vec_mergeh(tempB, tempJ);
    temp3 = vec_mergel(tempB, tempJ);
    temp4 = vec_mergeh(tempC, tempK);
    temp5 = vec_mergel(tempC, tempK);
    temp6 = vec_mergeh(tempD, tempL);
    temp7 = vec_mergel(tempD, tempL);
#define STORE_DOUBLE_LINE(i, j) do {                                    \
    vector unsigned char dstAi = vec_ld(i * stride, dst);               \
    vector unsigned char dstBi = vec_ld(i * stride + 16, dst);          \
    vector unsigned char dstAj = vec_ld(j * stride, dst);               \
    vector unsigned char dstBj = vec_ld(j * stride+ 16, dst);           \
    vector unsigned char aligni = vec_lvsr(i * stride, dst);            \
    vector unsigned char alignj = vec_lvsr(j * stride, dst);            \
    vector unsigned char maski =                                        \
        vec_perm(zero, (vector unsigned char)neg1, aligni);             \
    vector unsigned char maskj =                                        \
        vec_perm(zero, (vector unsigned char)neg1, alignj);             \
    vector unsigned char dstRi = vec_perm(temp##i, temp##i, aligni);    \
    vector unsigned char dstRj = vec_perm(temp##j, temp##j, alignj);    \
    vector unsigned char dstAFi = vec_sel(dstAi, dstRi, maski);         \
    vector unsigned char dstBFi = vec_sel(dstRi, dstBi, maski);         \
    vector unsigned char dstAFj = vec_sel(dstAj, dstRj, maskj);         \
    vector unsigned char dstBFj = vec_sel(dstRj, dstBj, maskj);         \
    vec_st(dstAFi, i * stride, dst);                                    \
    vec_st(dstBFi, i * stride + 16, dst);                               \
    vec_st(dstAFj, j * stride, dst);                                    \
    vec_st(dstBFj, j * stride + 16, dst);                               \
} while (0)
    STORE_DOUBLE_LINE(0,1);
    STORE_DOUBLE_LINE(2,3);
    STORE_DOUBLE_LINE(4,5);
    STORE_DOUBLE_LINE(6,7);
}
