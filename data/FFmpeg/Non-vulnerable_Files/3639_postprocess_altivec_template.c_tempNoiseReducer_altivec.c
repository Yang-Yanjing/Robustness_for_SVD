static inline void tempNoiseReducer_altivec(uint8_t *src, int stride,
                                            uint8_t *tempBlurred, uint32_t *tempBlurredPast, int *maxNoise)
{
    const vector signed char neg1 = vec_splat_s8(-1);
    const vector unsigned char permHH = (const vector unsigned char){0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                                     0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
    const vector signed int zero = vec_splat_s32(0);
    const vector signed short vsint16_1 = vec_splat_s16(1);
    vector signed int v_dp = zero;
    vector signed int v_sysdp = zero;
    int d, sysd, i;
#define LOAD_LINE(src, i)                                               \
    register int j##src##i = i * stride;                                \
    vector unsigned char perm##src##i = vec_lvsl(j##src##i, src);       \
    const vector unsigned char v_##src##A1##i = vec_ld(j##src##i, src); \
    const vector unsigned char v_##src##A2##i = vec_ld(j##src##i + 16, src); \
    const vector unsigned char v_##src##A##i =                          \
        vec_perm(v_##src##A1##i, v_##src##A2##i, perm##src##i);         \
    vector signed short v_##src##Ass##i =                               \
        (vector signed short)vec_mergeh((vector signed char)zero,       \
                                        (vector signed char)v_##src##A##i)
    LOAD_LINE(src, 0);
    LOAD_LINE(src, 1);
    LOAD_LINE(src, 2);
    LOAD_LINE(src, 3);
    LOAD_LINE(src, 4);
    LOAD_LINE(src, 5);
    LOAD_LINE(src, 6);
    LOAD_LINE(src, 7);
    LOAD_LINE(tempBlurred, 0);
    LOAD_LINE(tempBlurred, 1);
    LOAD_LINE(tempBlurred, 2);
    LOAD_LINE(tempBlurred, 3);
    LOAD_LINE(tempBlurred, 4);
    LOAD_LINE(tempBlurred, 5);
    LOAD_LINE(tempBlurred, 6);
    LOAD_LINE(tempBlurred, 7);
#undef LOAD_LINE
#define ACCUMULATE_DIFFS(i) do {                                \
        vector signed short v_d = vec_sub(v_tempBlurredAss##i,  \
                                          v_srcAss##i);         \
        v_dp = vec_msums(v_d, v_d, v_dp);                       \
        v_sysdp = vec_msums(v_d, vsint16_1, v_sysdp);           \
    } while (0)
    ACCUMULATE_DIFFS(0);
    ACCUMULATE_DIFFS(1);
    ACCUMULATE_DIFFS(2);
    ACCUMULATE_DIFFS(3);
    ACCUMULATE_DIFFS(4);
    ACCUMULATE_DIFFS(5);
    ACCUMULATE_DIFFS(6);
    ACCUMULATE_DIFFS(7);
#undef ACCUMULATE_DIFFS
    tempBlurredPast[127]= maxNoise[0];
    tempBlurredPast[128]= maxNoise[1];
    tempBlurredPast[129]= maxNoise[2];
    v_dp = vec_sums(v_dp, zero);
    v_sysdp = vec_sums(v_sysdp, zero);
    v_dp = vec_splat(v_dp, 3);
    v_sysdp = vec_splat(v_sysdp, 3);
    vec_ste(v_dp, 0, &d);
    vec_ste(v_sysdp, 0, &sysd);
    i = d;
    d = (4*d
         +(*(tempBlurredPast-256))
         +(*(tempBlurredPast-1))+ (*(tempBlurredPast+1))
         +(*(tempBlurredPast+256))
         +4)>>3;
    *tempBlurredPast=i;
    if (d > maxNoise[1]) {
        if (d < maxNoise[2]) {
#define OP(i) v_tempBlurredAss##i = vec_avg(v_tempBlurredAss##i, v_srcAss##i);
            OP(0);
            OP(1);
            OP(2);
            OP(3);
            OP(4);
            OP(5);
            OP(6);
            OP(7);
#undef OP
        } else {
#define OP(i) v_tempBlurredAss##i = v_srcAss##i;
            OP(0);
            OP(1);
            OP(2);
            OP(3);
            OP(4);
            OP(5);
            OP(6);
            OP(7);
#undef OP
        }
    } else {
        if (d < maxNoise[0]) {
            const vector signed short vsint16_7 = vec_splat_s16(7);
            const vector signed short vsint16_4 = vec_splat_s16(4);
            const vector unsigned short vuint16_3 = vec_splat_u16(3);
#define OP(i) do {                                                      \
            const vector signed short v_temp =                          \
                vec_mladd(v_tempBlurredAss##i, vsint16_7, v_srcAss##i); \
            const vector signed short v_temp2 = vec_add(v_temp, vsint16_4); \
            v_tempBlurredAss##i = vec_sr(v_temp2, vuint16_3);           \
        } while (0)
            OP(0);
            OP(1);
            OP(2);
            OP(3);
            OP(4);
            OP(5);
            OP(6);
            OP(7);
#undef OP
        } else {
            const vector signed short vsint16_3 = vec_splat_s16(3);
            const vector signed short vsint16_2 = vec_splat_s16(2);
#define OP(i) do {                                              \
            const vector signed short v_temp =                  \
                vec_mladd(v_tempBlurredAss##i, vsint16_3, v_srcAss##i); \
            const vector signed short v_temp2 = vec_add(v_temp, vsint16_2); \
            v_tempBlurredAss##i =                                       \
                vec_sr(v_temp2, (vector unsigned short)vsint16_2);      \
        } while (0)
            OP(0);
            OP(1);
            OP(2);
            OP(3);
            OP(4);
            OP(5);
            OP(6);
            OP(7);
#undef OP
        }
    }
#define PACK_AND_STORE(src, i) do {                                      \
    const vector unsigned char perms = vec_lvsr(i * stride, src);        \
    const vector unsigned char vf =                                      \
        vec_packsu(v_tempBlurredAss##1, (vector signed short)zero);     \
    const vector unsigned char vg = vec_perm(vf, v_##src##A##i, permHH); \
    const vector unsigned char mask =                                    \
        vec_perm((vector unsigned char)zero, (vector unsigned char)neg1, perms); \
    const vector unsigned char vg2 = vec_perm(vg, vg, perms);            \
    const vector unsigned char svA = vec_sel(v_##src##A1##i, vg2, mask); \
    const vector unsigned char svB = vec_sel(vg2, v_##src##A2##i, mask); \
    vec_st(svA, i * stride, src);                                        \
    vec_st(svB, i * stride + 16, src);                                   \
} while (0)
    PACK_AND_STORE(src, 0);
    PACK_AND_STORE(src, 1);
    PACK_AND_STORE(src, 2);
    PACK_AND_STORE(src, 3);
    PACK_AND_STORE(src, 4);
    PACK_AND_STORE(src, 5);
    PACK_AND_STORE(src, 6);
    PACK_AND_STORE(src, 7);
    PACK_AND_STORE(tempBlurred, 0);
    PACK_AND_STORE(tempBlurred, 1);
    PACK_AND_STORE(tempBlurred, 2);
    PACK_AND_STORE(tempBlurred, 3);
    PACK_AND_STORE(tempBlurred, 4);
    PACK_AND_STORE(tempBlurred, 5);
    PACK_AND_STORE(tempBlurred, 6);
    PACK_AND_STORE(tempBlurred, 7);
#undef PACK_AND_STORE
}
