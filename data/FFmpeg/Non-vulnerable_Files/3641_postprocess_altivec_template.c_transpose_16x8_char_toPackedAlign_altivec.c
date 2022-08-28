static inline void transpose_16x8_char_toPackedAlign_altivec(unsigned char* dst, unsigned char* src, int stride) {
    const vector unsigned char zero = vec_splat_u8(0);
#define LOAD_DOUBLE_LINE(i, j)                                          \
    vector unsigned char perm1##i = vec_lvsl(i * stride, src);          \
    vector unsigned char perm2##i = vec_lvsl(j * stride, src);          \
    vector unsigned char srcA##i = vec_ld(i * stride, src);             \
    vector unsigned char srcB##i = vec_ld(i * stride + 16, src);        \
    vector unsigned char srcC##i = vec_ld(j * stride, src);             \
    vector unsigned char srcD##i = vec_ld(j * stride+ 16, src);         \
    vector unsigned char src##i = vec_perm(srcA##i, srcB##i, perm1##i); \
    vector unsigned char src##j = vec_perm(srcC##i, srcD##i, perm2##i)
    LOAD_DOUBLE_LINE(0, 1);
    LOAD_DOUBLE_LINE(2, 3);
    LOAD_DOUBLE_LINE(4, 5);
    LOAD_DOUBLE_LINE(6, 7);
#undef LOAD_DOUBLE_LINE
    vector unsigned char tempA = vec_mergeh(src0, zero);
    vector unsigned char tempB = vec_mergel(src0, zero);
    vector unsigned char tempC = vec_mergeh(src1, zero);
    vector unsigned char tempD = vec_mergel(src1, zero);
    vector unsigned char tempE = vec_mergeh(src2, zero);
    vector unsigned char tempF = vec_mergel(src2, zero);
    vector unsigned char tempG = vec_mergeh(src3, zero);
    vector unsigned char tempH = vec_mergel(src3, zero);
    vector unsigned char tempI = vec_mergeh(src4, zero);
    vector unsigned char tempJ = vec_mergel(src4, zero);
    vector unsigned char tempK = vec_mergeh(src5, zero);
    vector unsigned char tempL = vec_mergel(src5, zero);
    vector unsigned char tempM = vec_mergeh(src6, zero);
    vector unsigned char tempN = vec_mergel(src6, zero);
    vector unsigned char tempO = vec_mergeh(src7, zero);
    vector unsigned char tempP = vec_mergel(src7, zero);
    vector unsigned char temp0  = vec_mergeh(tempA, tempI);
    vector unsigned char temp1  = vec_mergel(tempA, tempI);
    vector unsigned char temp2  = vec_mergeh(tempB, tempJ);
    vector unsigned char temp3  = vec_mergel(tempB, tempJ);
    vector unsigned char temp4  = vec_mergeh(tempC, tempK);
    vector unsigned char temp5  = vec_mergel(tempC, tempK);
    vector unsigned char temp6  = vec_mergeh(tempD, tempL);
    vector unsigned char temp7  = vec_mergel(tempD, tempL);
    vector unsigned char temp8  = vec_mergeh(tempE, tempM);
    vector unsigned char temp9  = vec_mergel(tempE, tempM);
    vector unsigned char temp10 = vec_mergeh(tempF, tempN);
    vector unsigned char temp11 = vec_mergel(tempF, tempN);
    vector unsigned char temp12 = vec_mergeh(tempG, tempO);
    vector unsigned char temp13 = vec_mergel(tempG, tempO);
    vector unsigned char temp14 = vec_mergeh(tempH, tempP);
    vector unsigned char temp15 = vec_mergel(tempH, tempP);
    tempA = vec_mergeh(temp0, temp8);
    tempB = vec_mergel(temp0, temp8);
    tempC = vec_mergeh(temp1, temp9);
    tempD = vec_mergel(temp1, temp9);
    tempE = vec_mergeh(temp2, temp10);
    tempF = vec_mergel(temp2, temp10);
    tempG = vec_mergeh(temp3, temp11);
    tempH = vec_mergel(temp3, temp11);
    tempI = vec_mergeh(temp4, temp12);
    tempJ = vec_mergel(temp4, temp12);
    tempK = vec_mergeh(temp5, temp13);
    tempL = vec_mergel(temp5, temp13);
    tempM = vec_mergeh(temp6, temp14);
    tempN = vec_mergel(temp6, temp14);
    tempO = vec_mergeh(temp7, temp15);
    tempP = vec_mergel(temp7, temp15);
    temp0  = vec_mergeh(tempA, tempI);
    temp1  = vec_mergel(tempA, tempI);
    temp2  = vec_mergeh(tempB, tempJ);
    temp3  = vec_mergel(tempB, tempJ);
    temp4  = vec_mergeh(tempC, tempK);
    temp5  = vec_mergel(tempC, tempK);
    temp6  = vec_mergeh(tempD, tempL);
    temp7  = vec_mergel(tempD, tempL);
    temp8  = vec_mergeh(tempE, tempM);
    temp9  = vec_mergel(tempE, tempM);
    temp10 = vec_mergeh(tempF, tempN);
    temp11 = vec_mergel(tempF, tempN);
    temp12 = vec_mergeh(tempG, tempO);
    temp13 = vec_mergel(tempG, tempO);
    temp14 = vec_mergeh(tempH, tempP);
    temp15 = vec_mergel(tempH, tempP);
    vec_st(temp0,    0, dst);
    vec_st(temp1,   16, dst);
    vec_st(temp2,   32, dst);
    vec_st(temp3,   48, dst);
    vec_st(temp4,   64, dst);
    vec_st(temp5,   80, dst);
    vec_st(temp6,   96, dst);
    vec_st(temp7,  112, dst);
    vec_st(temp8,  128, dst);
    vec_st(temp9,  144, dst);
    vec_st(temp10, 160, dst);
    vec_st(temp11, 176, dst);
    vec_st(temp12, 192, dst);
    vec_st(temp13, 208, dst);
    vec_st(temp14, 224, dst);
    vec_st(temp15, 240, dst);
}
