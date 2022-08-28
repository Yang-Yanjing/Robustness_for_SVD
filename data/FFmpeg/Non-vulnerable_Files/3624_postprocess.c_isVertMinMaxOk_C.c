static inline int isVertMinMaxOk_C(const uint8_t src[], int stride, int QP)
{
    int x;
    src+= stride*4;
    for(x=0; x<BLOCK_SIZE; x+=4){
        if((unsigned)(src[  x + 0*stride] - src[  x + 5*stride] + 2*QP) > 4*QP) return 0;
        if((unsigned)(src[1+x + 2*stride] - src[1+x + 7*stride] + 2*QP) > 4*QP) return 0;
        if((unsigned)(src[2+x + 4*stride] - src[2+x + 1*stride] + 2*QP) > 4*QP) return 0;
        if((unsigned)(src[3+x + 6*stride] - src[3+x + 3*stride] + 2*QP) > 4*QP) return 0;
    }
    return 1;
}
