void ff_vector_fmul_add_vsx(float *dst, const float *src0,
                            const float *src1, const float *src2,
                            int len)
{
    int i;
    vec_f d, s0, s1, s2;
    for (i = 0; i < len - 3; i += 4) {
        s0 = vec_vsx_ld(0, src0 + i);
        s1 = vec_vsx_ld(0, src1 + i);
        s2 = vec_vsx_ld(0, src2 + i);
        d = vec_madd(s0, s1, s2);
        vec_vsx_st(d, 0, dst + i);
    }
}
