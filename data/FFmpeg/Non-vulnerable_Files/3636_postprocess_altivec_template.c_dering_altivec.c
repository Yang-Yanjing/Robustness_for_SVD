static inline void dering_altivec(uint8_t src[], int stride, PPContext *c) {
    const vector signed int vsint32_8 = vec_splat_s32(8);
    const vector unsigned int vuint32_4 = vec_splat_u32(4);
    const vector signed char neg1 = vec_splat_s8(-1);
    const vector unsigned char permA1 = (vector unsigned char)
        {0x00, 0x01, 0x02, 0x10, 0x11, 0x12, 0x1F, 0x1F,
         0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
    const vector unsigned char permA2 = (vector unsigned char)
        {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x10, 0x11,
         0x12, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
    const vector unsigned char permA1inc = (vector unsigned char)
        {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const vector unsigned char permA2inc = (vector unsigned char)
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
         0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const vector unsigned char magic = (vector unsigned char)
        {0x01, 0x02, 0x01, 0x02, 0x04, 0x02, 0x01, 0x02,
         0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const vector unsigned char extractPerm = (vector unsigned char)
        {0x10, 0x10, 0x10, 0x01, 0x10, 0x10, 0x10, 0x01,
         0x10, 0x10, 0x10, 0x01, 0x10, 0x10, 0x10, 0x01};
    const vector unsigned char extractPermInc = (vector unsigned char)
        {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
         0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01};
    const vector unsigned char identity = vec_lvsl(0,(unsigned char *)0);
    const vector unsigned char tenRight = (vector unsigned char)
        {0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const vector unsigned char eightLeft = (vector unsigned char)
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08};
    







    uint8_t *srcCopy = src;
    DECLARE_ALIGNED(16, uint8_t, dt)[16] = { deringThreshold };
    const vector signed int zero = vec_splat_s32(0);
    vector unsigned char v_dt = vec_splat(vec_ld(0, dt), 0);
#define LOAD_LINE(i)                                                  \
    const vector unsigned char perm##i =                              \
        vec_lvsl(i * stride, srcCopy);                                \
    vector unsigned char sA##i = vec_ld(i * stride, srcCopy);         \
    vector unsigned char sB##i = vec_ld(i * stride + 16, srcCopy);    \
    vector unsigned char src##i = vec_perm(sA##i, sB##i, perm##i)
    LOAD_LINE(0);
    LOAD_LINE(1);
    LOAD_LINE(2);
    LOAD_LINE(3);
    LOAD_LINE(4);
    LOAD_LINE(5);
    LOAD_LINE(6);
    LOAD_LINE(7);
    LOAD_LINE(8);
    LOAD_LINE(9);
#undef LOAD_LINE
    vector unsigned char v_avg;
    DECLARE_ALIGNED(16, signed int, S)[8];
    DECLARE_ALIGNED(16, int, tQP2)[4] = { c->QP/2 + 1 };
    vector signed int vQP2 = vec_ld(0, tQP2);
    vQP2 = vec_splat(vQP2, 0);
    {
    const vector unsigned char trunc_perm = (vector unsigned char)
        {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
         0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18};
    const vector unsigned char trunc_src12 = vec_perm(src1, src2, trunc_perm);
    const vector unsigned char trunc_src34 = vec_perm(src3, src4, trunc_perm);
    const vector unsigned char trunc_src56 = vec_perm(src5, src6, trunc_perm);
    const vector unsigned char trunc_src78 = vec_perm(src7, src8, trunc_perm);
#define EXTRACT(op) do {                                                \
    const vector unsigned char s_1   = vec_##op(trunc_src12, trunc_src34); \
    const vector unsigned char s_2   = vec_##op(trunc_src56, trunc_src78); \
    const vector unsigned char s_6   = vec_##op(s_1, s_2);     \
    const vector unsigned char s_8h  = vec_mergeh(s_6, s_6);   \
    const vector unsigned char s_8l  = vec_mergel(s_6, s_6);   \
    const vector unsigned char s_9   = vec_##op(s_8h, s_8l);   \
    const vector unsigned char s_9h  = vec_mergeh(s_9, s_9);   \
    const vector unsigned char s_9l  = vec_mergel(s_9, s_9);   \
    const vector unsigned char s_10  = vec_##op(s_9h, s_9l);   \
    const vector unsigned char s_10h = vec_mergeh(s_10, s_10); \
    const vector unsigned char s_10l = vec_mergel(s_10, s_10); \
    const vector unsigned char s_11  = vec_##op(s_10h, s_10l); \
    const vector unsigned char s_11h = vec_mergeh(s_11, s_11); \
    const vector unsigned char s_11l = vec_mergel(s_11, s_11); \
    v_##op = vec_##op(s_11h, s_11l);                           \
} while (0)
    vector unsigned char v_min;
    vector unsigned char v_max;
    EXTRACT(min);
    EXTRACT(max);
#undef EXTRACT
    if (vec_all_lt(vec_sub(v_max, v_min), v_dt))
        return;
    v_avg = vec_avg(v_min, v_max);
    }
    {
    const vector unsigned short mask1 = (vector unsigned short)
                                        {0x0001, 0x0002, 0x0004, 0x0008,
                                         0x0010, 0x0020, 0x0040, 0x0080};
    const vector unsigned short mask2 = (vector unsigned short)
                                        {0x0100, 0x0200, 0x0000, 0x0000,
                                         0x0000, 0x0000, 0x0000, 0x0000};
    const vector unsigned int vuint32_16 = vec_sl(vec_splat_u32(1), vec_splat_u32(4));
    const vector unsigned int vuint32_1 = vec_splat_u32(1);
    vector signed int sumA2;
    vector signed int sumB2;
    vector signed int sum0, sum1, sum2, sum3, sum4;
    vector signed int sum5, sum6, sum7, sum8, sum9;
#define COMPARE(i)                                                      \
    do {                                                                \
        const vector unsigned char cmp =                                \
            (vector unsigned char)vec_cmpgt(src##i, v_avg);             \
        const vector unsigned short cmpHi =                             \
            (vector unsigned short)vec_mergeh(cmp, cmp);                \
        const vector unsigned short cmpLi =                             \
            (vector unsigned short)vec_mergel(cmp, cmp);                \
        const vector signed short cmpHf =                               \
            (vector signed short)vec_and(cmpHi, mask1);                 \
        const vector signed short cmpLf =                               \
            (vector signed short)vec_and(cmpLi, mask2);                 \
        const vector signed int sump = vec_sum4s(cmpHf, zero);          \
        const vector signed int sumq = vec_sum4s(cmpLf, sump);          \
        sum##i  = vec_sums(sumq, zero);                                 \
    } while (0)
    COMPARE(0);
    COMPARE(1);
    COMPARE(2);
    COMPARE(3);
    COMPARE(4);
    COMPARE(5);
    COMPARE(6);
    COMPARE(7);
    COMPARE(8);
    COMPARE(9);
#undef COMPARE
    {
    const vector signed int sump02 = vec_mergel(sum0, sum2);
    const vector signed int sump13 = vec_mergel(sum1, sum3);
    const vector signed int sumA = vec_mergel(sump02, sump13);
    const vector signed int sump46 = vec_mergel(sum4, sum6);
    const vector signed int sump57 = vec_mergel(sum5, sum7);
    const vector signed int sumB = vec_mergel(sump46, sump57);
    const vector signed int sump8A = vec_mergel(sum8, zero);
    const vector signed int sump9B = vec_mergel(sum9, zero);
    const vector signed int sumC = vec_mergel(sump8A, sump9B);
    const vector signed int tA = vec_sl(vec_nor(zero, sumA), vuint32_16);
    const vector signed int tB = vec_sl(vec_nor(zero, sumB), vuint32_16);
    const vector signed int tC = vec_sl(vec_nor(zero, sumC), vuint32_16);
    const vector signed int t2A = vec_or(sumA, tA);
    const vector signed int t2B = vec_or(sumB, tB);
    const vector signed int t2C = vec_or(sumC, tC);
    const vector signed int t3A = vec_and(vec_sra(t2A, vuint32_1),
                                          vec_sl(t2A, vuint32_1));
    const vector signed int t3B = vec_and(vec_sra(t2B, vuint32_1),
                                          vec_sl(t2B, vuint32_1));
    const vector signed int t3C = vec_and(vec_sra(t2C, vuint32_1),
                                          vec_sl(t2C, vuint32_1));
    const vector signed int yA = vec_and(t2A, t3A);
    const vector signed int yB = vec_and(t2B, t3B);
    const vector signed int yC = vec_and(t2C, t3C);
    const vector unsigned char strangeperm1 = vec_lvsl(4, (unsigned char*)0);
    const vector unsigned char strangeperm2 = vec_lvsl(8, (unsigned char*)0);
    const vector signed int sumAd4 = vec_perm(yA, yB, strangeperm1);
    const vector signed int sumAd8 = vec_perm(yA, yB, strangeperm2);
    const vector signed int sumBd4 = vec_perm(yB, yC, strangeperm1);
    const vector signed int sumBd8 = vec_perm(yB, yC, strangeperm2);
    const vector signed int sumAp = vec_and(yA,
                                            vec_and(sumAd4,sumAd8));
    const vector signed int sumBp = vec_and(yB,
                                            vec_and(sumBd4,sumBd8));
    sumA2 = vec_or(sumAp,
                   vec_sra(sumAp,
                           vuint32_16));
    sumB2  = vec_or(sumBp,
                    vec_sra(sumBp,
                            vuint32_16));
    }
    vec_st(sumA2, 0, S);
    vec_st(sumB2, 16, S);
    }
    

#define F_INIT()                                       \
    vector unsigned char tenRightM = tenRight;         \
    vector unsigned char permA1M = permA1;             \
    vector unsigned char permA2M = permA2;             \
    vector unsigned char extractPermM = extractPerm
#define F2(i, j, k, l)                                                  \
    if (S[i] & (1 << (l+1))) {                                          \
        const vector unsigned char a_A = vec_perm(src##i, src##j, permA1M); \
        const vector unsigned char a_B = vec_perm(a_A, src##k, permA2M); \
        const vector signed int a_sump =                                \
            (vector signed int)vec_msum(a_B, magic, (vector unsigned int)zero);\
        vector signed int F = vec_sr(vec_sums(a_sump, vsint32_8), vuint32_4); \
        const vector signed int p =                                     \
            (vector signed int)vec_perm(src##j, (vector unsigned char)zero, \
                                        extractPermM);                  \
        const vector signed int sum  = vec_add(p, vQP2);                \
        const vector signed int diff = vec_sub(p, vQP2);                \
        vector signed int newpm;                                        \
        vector unsigned char newpm2, mask;                              \
        F = vec_splat(F, 3);                                            \
        if (vec_all_lt(sum, F))                                         \
            newpm = sum;                                                \
        else if (vec_all_gt(diff, F))                                   \
            newpm = diff;                                               \
        else newpm = F;                                                 \
        newpm2 = vec_splat((vector unsigned char)newpm, 15);            \
        mask = vec_add(identity, tenRightM);                            \
        src##j = vec_perm(src##j, newpm2, mask);                        \
    }                                                                   \
    permA1M = vec_add(permA1M, permA1inc);                              \
    permA2M = vec_add(permA2M, permA2inc);                              \
    tenRightM = vec_sro(tenRightM, eightLeft);                          \
    extractPermM = vec_add(extractPermM, extractPermInc)
#define ITER(i, j, k) do {                      \
    F_INIT();                                   \
    F2(i, j, k, 0);                             \
    F2(i, j, k, 1);                             \
    F2(i, j, k, 2);                             \
    F2(i, j, k, 3);                             \
    F2(i, j, k, 4);                             \
    F2(i, j, k, 5);                             \
    F2(i, j, k, 6);                             \
    F2(i, j, k, 7);                             \
} while (0)
    ITER(0, 1, 2);
    ITER(1, 2, 3);
    ITER(2, 3, 4);
    ITER(3, 4, 5);
    ITER(4, 5, 6);
    ITER(5, 6, 7);
    ITER(6, 7, 8);
    ITER(7, 8, 9);
#define STORE_LINE(i) do {                              \
    const vector unsigned char permST =                 \
        vec_lvsr(i * stride, srcCopy);                  \
    const vector unsigned char maskST =                 \
        vec_perm((vector unsigned char)zero,            \
                 (vector unsigned char)neg1, permST);   \
    src##i = vec_perm(src##i ,src##i, permST);          \
    sA##i= vec_sel(sA##i, src##i, maskST);              \
    sB##i= vec_sel(src##i, sB##i, maskST);              \
    vec_st(sA##i, i * stride, srcCopy);                 \
    vec_st(sB##i, i * stride + 16, srcCopy);            \
} while (0)
    STORE_LINE(1);
    STORE_LINE(2);
    STORE_LINE(3);
    STORE_LINE(4);
    STORE_LINE(5);
    STORE_LINE(6);
    STORE_LINE(7);
    STORE_LINE(8);
#undef STORE_LINE
#undef ITER
#undef F2
}
