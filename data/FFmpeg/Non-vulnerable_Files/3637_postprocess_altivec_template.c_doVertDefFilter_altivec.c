static inline void doVertDefFilter_altivec(uint8_t src[], int stride, PPContext *c) {
    







    uint8_t *src2 = src + stride*3;
    const vector signed int zero = vec_splat_s32(0);
    DECLARE_ALIGNED(16, short, qp)[8] = {8*c->QP};
    vector signed short vqp = vec_splat(
                                (vector signed short)vec_ld(0, qp), 0);
#define LOAD_LINE(i)                                                    \
    const vector unsigned char perm##i =                                \
        vec_lvsl(i * stride, src2);                                     \
    const vector unsigned char vbA##i =                                 \
        vec_ld(i * stride, src2);                                       \
    const vector unsigned char vbB##i =                                 \
        vec_ld(i * stride + 16, src2);                                  \
    const vector unsigned char vbT##i =                                 \
        vec_perm(vbA##i, vbB##i, perm##i);                              \
    const vector signed short vb##i =                                   \
        (vector signed short)vec_mergeh((vector unsigned char)zero,     \
                                        (vector unsigned char)vbT##i)
     LOAD_LINE(1);
     LOAD_LINE(2);
     LOAD_LINE(3);
     LOAD_LINE(4);
     LOAD_LINE(5);
     LOAD_LINE(6);
     LOAD_LINE(7);
     LOAD_LINE(8);
#undef LOAD_LINE
     const vector signed short v_1 = vec_splat_s16(1);
     const vector signed short v_2 = vec_splat_s16(2);
     const vector signed short v_5 = vec_splat_s16(5);
     const vector signed short v_32 = vec_sl(v_1,
                                             (vector unsigned short)v_5);
     
     const vector signed short l3minusl6 = vec_sub(vb3, vb6);
     const vector signed short l5minusl4 = vec_sub(vb5, vb4);
     const vector signed short twotimes_l3minusl6 = vec_mladd(v_2, l3minusl6, (vector signed short)zero);
     const vector signed short mE = vec_mladd(v_5, l5minusl4, twotimes_l3minusl6);
     const vector signed short absmE = vec_abs(mE);
     
     const vector signed short l1minusl4 = vec_sub(vb1, vb4);
     const vector signed short l3minusl2 = vec_sub(vb3, vb2);
     const vector signed short l5minusl8 = vec_sub(vb5, vb8);
     const vector signed short l7minusl6 = vec_sub(vb7, vb6);
     const vector signed short twotimes_l1minusl4 = vec_mladd(v_2, l1minusl4, (vector signed short)zero);
     const vector signed short twotimes_l5minusl8 = vec_mladd(v_2, l5minusl8, (vector signed short)zero);
     const vector signed short lE = vec_mladd(v_5, l3minusl2, twotimes_l1minusl4);
     const vector signed short rE = vec_mladd(v_5, l7minusl6, twotimes_l5minusl8);
     
     const vector signed short ddiff = vec_sub(absmE,
                                               vec_min(vec_abs(lE),
                                                       vec_abs(rE)));
     const vector signed short ddiffclamp = vec_max(ddiff, (vector signed short)zero);
     const vector signed short dtimes64 = vec_mladd(v_5, ddiffclamp, v_32);
     const vector signed short d = vec_sra(dtimes64, vec_splat_u16(6));
     const vector signed short minusd = vec_sub((vector signed short)zero, d);
     const vector signed short finald = vec_sel(minusd,
                                                d,
                                                vec_cmpgt(vec_sub((vector signed short)zero, mE),
                                                          (vector signed short)zero));
     
     const vector signed short qtimes2 = vec_sub(vb4, vb5);
     

     const vector signed short rounddown = vec_sel((vector signed short)zero,
                                                   v_1,
                                                   vec_cmplt(qtimes2, (vector signed short)zero));
     const vector signed short q = vec_sra(vec_add(qtimes2, rounddown), vec_splat_u16(1));
     
     const vector signed short dclamp_P1 = vec_max((vector signed short)zero, finald);
     const vector signed short dclamp_P = vec_min(dclamp_P1, q);
     const vector signed short dclamp_N1 = vec_min((vector signed short)zero, finald);
     const vector signed short dclamp_N = vec_max(dclamp_N1, q);
     const vector signed short dclampedfinal = vec_sel(dclamp_N,
                                                       dclamp_P,
                                                       vec_cmpgt(q, (vector signed short)zero));
     const vector signed short dornotd = vec_sel((vector signed short)zero,
                                                 dclampedfinal,
                                                 vec_cmplt(absmE, vqp));
     
     const vector signed short vb4minusd = vec_sub(vb4, dornotd);
     const vector signed short vb5plusd  = vec_add(vb5, dornotd);
     
     const vector unsigned char st4 = vec_packsu(vb4minusd, (vector signed short)zero);
     const vector unsigned char st5 = vec_packsu(vb5plusd,  (vector signed short)zero);
     const vector signed char neg1 = vec_splat_s8(-1);
     const vector unsigned char permHH = (const vector unsigned char){0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                                      0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
#define STORE(i)                                                \
{    const vector unsigned char perms##i =                      \
         vec_lvsr(i * stride, src2);                            \
     const vector unsigned char vg##i =                         \
         vec_perm(st##i, vbT##i, permHH);                       \
     const vector unsigned char mask##i =                       \
         vec_perm((vector unsigned char)zero, (vector unsigned char)neg1, perms##i); \
     const vector unsigned char vg2##i =                        \
         vec_perm(vg##i, vg##i, perms##i);                      \
     const vector unsigned char svA##i =                        \
         vec_sel(vbA##i, vg2##i, mask##i);                      \
     const vector unsigned char svB##i =                        \
         vec_sel(vg2##i, vbB##i, mask##i);                      \
     vec_st(svA##i, i * stride, src2);                          \
     vec_st(svB##i, i * stride + 16, src2);}
     STORE(4)
     STORE(5)
}
