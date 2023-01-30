static void yuv2planeX_16_altivec(const int16_t *filter, int filterSize,
                                  const int16_t **src, uint8_t *dest,
                                  const uint8_t *dither, int offset, int x)
{
    register int i, j;
    LOCAL_ALIGNED(16, int, val, [16]);
    vector signed int vo1, vo2, vo3, vo4;
    vector unsigned short vs1, vs2;
    vector unsigned char vf;
    vector unsigned int altivec_vectorShiftInt19 =
        vec_add(vec_splat_u32(10), vec_splat_u32(9));
    for (i = 0; i < 16; i++)
        val[i] = dither[(x + i + offset) & 7] << 12;
    vo1 = vec_ld(0,  val);
    vo2 = vec_ld(16, val);
    vo3 = vec_ld(32, val);
    vo4 = vec_ld(48, val);
    for (j = 0; j < filterSize; j++) {
        unsigned int joffset=j<<1;
        unsigned int xoffset=x<<1;
        vector unsigned char perm;
        vector signed short l1,vLumFilter;
        LOAD_FILTER(vLumFilter,filter);
        vLumFilter = vec_splat(vLumFilter, 0);
        LOAD_L1(l1,src[j],perm);
        yuv2planeX_8(vo1, vo2, l1, src[j], x,     perm, vLumFilter);
        yuv2planeX_8(vo3, vo4, l1, src[j], x + 8, perm, vLumFilter);
    }
    vo1 = vec_sra(vo1, altivec_vectorShiftInt19);
    vo2 = vec_sra(vo2, altivec_vectorShiftInt19);
    vo3 = vec_sra(vo3, altivec_vectorShiftInt19);
    vo4 = vec_sra(vo4, altivec_vectorShiftInt19);
    vs1 = vec_packsu(vo1, vo2);
    vs2 = vec_packsu(vo3, vo4);
    vf  = vec_packsu(vs1, vs2);
    VEC_ST(vf, 0, dest);
}
