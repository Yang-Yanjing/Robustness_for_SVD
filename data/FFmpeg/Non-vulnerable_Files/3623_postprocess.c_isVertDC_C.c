static inline int isVertDC_C(const uint8_t src[], int stride, const PPContext *c)
{
    int numEq= 0;
    int y;
    const int dcOffset= ((c->nonBQP*c->ppMode.baseDcDiff)>>8) + 1;
    const int dcThreshold= dcOffset*2 + 1;
    src+= stride*4; 
    for(y=0; y<BLOCK_SIZE-1; y++){
        numEq += ((unsigned)(src[0] - src[0+stride] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[1] - src[1+stride] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[2] - src[2+stride] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[3] - src[3+stride] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[4] - src[4+stride] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[5] - src[5+stride] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[6] - src[6+stride] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[7] - src[7+stride] + dcOffset)) < dcThreshold;
        src+= stride;
    }
    return numEq > c->ppMode.flatnessThreshold;
}
