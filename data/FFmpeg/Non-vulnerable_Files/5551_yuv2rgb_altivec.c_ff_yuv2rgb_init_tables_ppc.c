av_cold void ff_yuv2rgb_init_tables_ppc(SwsContext *c,
                                        const int inv_table[4],
                                        int brightness,
                                        int contrast,
                                        int saturation)
{
#if HAVE_ALTIVEC
    union {
        DECLARE_ALIGNED(16, signed short, tmp)[8];
        vector signed short vec;
    } buf;
    if (!(av_get_cpu_flags() & AV_CPU_FLAG_ALTIVEC))
        return;
    buf.tmp[0] = ((0xffffLL) * contrast >> 8) >> 9;                               
    buf.tmp[1] = -256 * brightness;                                               
    buf.tmp[2] =   (inv_table[0] >> 3) * (contrast >> 16) * (saturation >> 16);   
    buf.tmp[3] =   (inv_table[1] >> 3) * (contrast >> 16) * (saturation >> 16);   
    buf.tmp[4] = -((inv_table[2] >> 1) * (contrast >> 16) * (saturation >> 16));  
    buf.tmp[5] = -((inv_table[3] >> 1) * (contrast >> 16) * (saturation >> 16));  
    c->CSHIFT = (vector unsigned short) vec_splat_u16(2);
    c->CY     = vec_splat((vector signed short) buf.vec, 0);
    c->OY     = vec_splat((vector signed short) buf.vec, 1);
    c->CRV    = vec_splat((vector signed short) buf.vec, 2);
    c->CBU    = vec_splat((vector signed short) buf.vec, 3);
    c->CGU    = vec_splat((vector signed short) buf.vec, 4);
    c->CGV    = vec_splat((vector signed short) buf.vec, 5);
    return;
#endif 
}
