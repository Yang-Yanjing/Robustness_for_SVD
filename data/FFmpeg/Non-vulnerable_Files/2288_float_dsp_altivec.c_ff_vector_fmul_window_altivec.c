void ff_vector_fmul_window_altivec(float *dst, const float *src0,
                                   const float *src1, const float *win, int len)
{
    vec_f zero, t0, t1, s0, s1, wi, wj;
    const vec_u8 reverse = vcprm(3, 2, 1, 0);
    int i, j;
    dst  += len;
    win  += len;
    src0 += len;
    zero = (vec_f)vec_splat_u32(0);
    for (i = -len * 4, j = len * 4 - 16; i < 0; i += 16, j -= 16) {
        s0 = vec_ld(i, src0);
        s1 = vec_ld(j, src1);
        wi = vec_ld(i, win);
        wj = vec_ld(j, win);
        s1 = vec_perm(s1, s1, reverse);
        wj = vec_perm(wj, wj, reverse);
        t0 = vec_madd(s0, wj, zero);
        t0 = vec_nmsub(s1, wi, t0);
        t1 = vec_madd(s0, wi, zero);
        t1 = vec_madd(s1, wj, t1);
        t1 = vec_perm(t1, t1, reverse);
        vec_st(t0, i, dst);
        vec_st(t1, j, dst);
    }
}
