static inline int isHorizMinMaxOk_C(const uint8_t src[], int stride, int QP)
{
    int i;
    for(i=0; i<2; i++){
        if((unsigned)(src[0] - src[5] + 2*QP) > 4*QP) return 0;
        src += stride;
        if((unsigned)(src[2] - src[7] + 2*QP) > 4*QP) return 0;
        src += stride;
        if((unsigned)(src[4] - src[1] + 2*QP) > 4*QP) return 0;
        src += stride;
        if((unsigned)(src[6] - src[3] + 2*QP) > 4*QP) return 0;
        src += stride;
    }
    return 1;
}
