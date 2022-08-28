static inline int isHorizDC_C(const uint8_t src[], int stride, const PPContext *c)
{
    int numEq= 0;
    int y;
    const int dcOffset= ((c->nonBQP*c->ppMode.baseDcDiff)>>8) + 1;
    const int dcThreshold= dcOffset*2 + 1;
    for(y=0; y<BLOCK_SIZE; y++){
        numEq += ((unsigned)(src[0] - src[1] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[1] - src[2] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[2] - src[3] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[3] - src[4] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[4] - src[5] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[5] - src[6] + dcOffset)) < dcThreshold;
        numEq += ((unsigned)(src[6] - src[7] + dcOffset)) < dcThreshold;
        src+= stride;
    }
    return numEq > c->ppMode.flatnessThreshold;
}
