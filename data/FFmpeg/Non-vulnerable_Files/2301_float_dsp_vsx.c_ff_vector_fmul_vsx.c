void ff_vector_fmul_vsx(float *dst,
                        const float *src0, const float *src1,
                        int len)
{
    int i;
    vec_f d0, d1, zero = (vec_f)vec_splat_u32(0);
    for (i = 0; i < len - 7; i += 8) {
        d0 = vec_vsx_ld( 0, src0 + i);
        d1 = vec_vsx_ld(16, src0 + i);
        d0 = vec_madd(d0, vec_vsx_ld( 0, src1 + i), zero);
        d1 = vec_madd(d1, vec_vsx_ld(16, src1 + i), zero);
        vec_vsx_st(d0,  0, dst + i);
        vec_vsx_st(d1, 16, dst + i);
    }
}
