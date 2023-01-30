static void hScale_altivec_real(SwsContext *c, int16_t *dst, int dstW,
                                const uint8_t *src, const int16_t *filter,
                                const int32_t *filterPos, int filterSize)
{
    register int i;
    LOCAL_ALIGNED(16, int, tempo, [4]);
    if (filterSize % 4) {
        for (i = 0; i < dstW; i++) {
            register int j;
            register int srcPos = filterPos[i];
            register int val    = 0;
            for (j = 0; j < filterSize; j++)
                val += ((int)src[srcPos + j]) * filter[filterSize * i + j];
            dst[i] = FFMIN(val >> 7, (1 << 15) - 1);
        }
    } else
        switch (filterSize) {
        case 4:
            for (i = 0; i < dstW; i++) {
                register int srcPos = filterPos[i];
                vector unsigned char src_vF = unaligned_load(srcPos, src);
                vector signed short src_v, filter_v;
                vector signed int val_vEven, val_s;
                src_v = 
                        (vector signed short)(VEC_MERGEH((vector unsigned char)vzero, src_vF));
                
                src_v = vec_mergeh(src_v, (vector signed short)vzero);
                GET_VF4(i, filter_v, filter);
                val_vEven = vec_mule(src_v, filter_v);
                val_s     = vec_sums(val_vEven, vzero);
                vec_st(val_s, 0, tempo);
                dst[i] = FFMIN(tempo[3] >> 7, (1 << 15) - 1);
            }
        break;
        case 8:
            for (i = 0; i < dstW; i++) {
                register int srcPos = filterPos[i];
                vector unsigned char src_vF, src_v0, src_v1;
                vector unsigned char permS;
                vector signed short src_v, filter_v;
                vector signed int val_v, val_s;
                FIRST_LOAD(src_v0, srcPos, src, permS);
                LOAD_SRCV8(srcPos, 0, src, permS, src_v0, src_v1, src_vF);
                src_v = 
                        (vector signed short)(VEC_MERGEH((vector unsigned char)vzero, src_vF));
                filter_v = vec_ld(i << 4, filter);
                val_v = vec_msums(src_v, filter_v, (vector signed int)vzero);
                val_s = vec_sums(val_v, vzero);
                vec_st(val_s, 0, tempo);
                dst[i] = FFMIN(tempo[3] >> 7, (1 << 15) - 1);
            }
        break;
        case 16:
            for (i = 0; i < dstW; i++) {
                register int srcPos = filterPos[i];
                vector unsigned char src_vF = unaligned_load(srcPos, src);
                vector signed short src_vA = 
                                             (vector signed short)(VEC_MERGEH((vector unsigned char)vzero, src_vF));
                vector signed short src_vB = 
                                             (vector signed short)(VEC_MERGEL((vector unsigned char)vzero, src_vF));
                vector signed short filter_v0 = vec_ld(i << 5, filter);
                vector signed short filter_v1 = vec_ld((i << 5) + 16, filter);
                vector signed int val_acc = vec_msums(src_vA, filter_v0, (vector signed int)vzero);
                vector signed int val_v   = vec_msums(src_vB, filter_v1, val_acc);
                vector signed int val_s = vec_sums(val_v, vzero);
                VEC_ST(val_s, 0, tempo);
                dst[i] = FFMIN(tempo[3] >> 7, (1 << 15) - 1);
            }
        break;
        default:
            for (i = 0; i < dstW; i++) {
                register int j, offset = i * 2 * filterSize;
                register int srcPos = filterPos[i];
                vector signed int val_s, val_v = (vector signed int)vzero;
                vector signed short filter_v0R;
                vector unsigned char permF, src_v0, permS;
                FIRST_LOAD(filter_v0R, offset, filter, permF);
                FIRST_LOAD(src_v0, srcPos, src, permS);
                for (j = 0; j < filterSize - 15; j += 16) {
                    vector unsigned char src_v1, src_vF;
                    vector signed short filter_v1R, filter_v2R, filter_v0, filter_v1;
                    LOAD_SRCV(srcPos, j, src, permS, src_v0, src_v1, src_vF);
                    vector signed short src_vA = 
                                                 (vector signed short)(VEC_MERGEH((vector unsigned char)vzero, src_vF));
                    vector signed short src_vB = 
                                                 (vector signed short)(VEC_MERGEL((vector unsigned char)vzero, src_vF));
                    GET_VFD(i, j, filter, filter_v0R, filter_v1R, permF, filter_v0, 0);
                    GET_VFD(i, j, filter, filter_v1R, filter_v2R, permF, filter_v1, 16);
                    vector signed int val_acc = vec_msums(src_vA, filter_v0, val_v);
                    val_v = vec_msums(src_vB, filter_v1, val_acc);
                    UPDATE_PTR(filter_v2R, filter_v0R, src_v1, src_v0);
                }
                if (j < filterSize - 7) {
                    
                    vector unsigned char src_v1, src_vF;
                    vector signed short src_v, filter_v1R, filter_v;
                    LOAD_SRCV8(srcPos, j, src, permS, src_v0, src_v1, src_vF);
                    src_v = 
                            (vector signed short)(VEC_MERGEH((vector unsigned char)vzero, src_vF));
                    GET_VFD(i, j, filter, filter_v0R, filter_v1R, permF, filter_v, 0);
                    val_v = vec_msums(src_v, filter_v, val_v);
                }
                val_s = vec_sums(val_v, vzero);
                VEC_ST(val_s, 0, tempo);
                dst[i] = FFMIN(tempo[3] >> 7, (1 << 15) - 1);
            }
        }
}
#endif 
static void hScale_altivec_real(SwsContext *c, int16_t *dst, int dstW,
                                const uint8_t *src, const int16_t *filter,
                                const int32_t *filterPos, int filterSize)
{
    register int i;
    LOCAL_ALIGNED(16, int, tempo, [4]);
    if (filterSize % 4) {
        for (i = 0; i < dstW; i++) {
            register int j;
            register int srcPos = filterPos[i];
            register int val    = 0;
            for (j = 0; j < filterSize; j++)
                val += ((int)src[srcPos + j]) * filter[filterSize * i + j];
            dst[i] = FFMIN(val >> 7, (1 << 15) - 1);
        }
    } else
#endif 
static void hScale_altivec_real(SwsContext *c, int16_t *dst, int dstW,
                                const uint8_t *src, const int16_t *filter,
                                const int32_t *filterPos, int filterSize)
{
    register int i;
    LOCAL_ALIGNED(16, int, tempo, [4]);
    if (filterSize % 4) {
        for (i = 0; i < dstW; i++) {
            register int j;
            register int srcPos = filterPos[i];
            register int val    = 0;
            for (j = 0; j < filterSize; j++)
                val += ((int)src[srcPos + j]) * filter[filterSize * i + j];
            dst[i] = FFMIN(val >> 7, (1 << 15) - 1);
        }
