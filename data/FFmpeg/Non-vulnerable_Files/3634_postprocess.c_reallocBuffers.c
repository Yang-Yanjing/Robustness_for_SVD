static void reallocBuffers(PPContext *c, int width, int height, int stride, int qpStride){
    int mbWidth = (width+15)>>4;
    int mbHeight= (height+15)>>4;
    int i;
    c->stride= stride;
    c->qpStride= qpStride;
    reallocAlign((void **)&c->tempDst, stride*24+32);
    reallocAlign((void **)&c->tempSrc, stride*24);
    reallocAlign((void **)&c->tempBlocks, 2*16*8);
    reallocAlign((void **)&c->yHistogram, 256*sizeof(uint64_t));
    for(i=0; i<256; i++)
            c->yHistogram[i]= width*height/64*15/256;
    for(i=0; i<3; i++){
        
        reallocAlign((void **)&c->tempBlurred[i], stride*mbHeight*16 + 17*1024);
        reallocAlign((void **)&c->tempBlurredPast[i], 256*((height+7)&(~7))/2 + 17*1024);
    }
    reallocAlign((void **)&c->deintTemp, 2*width+32);
    reallocAlign((void **)&c->nonBQPTable, qpStride*mbHeight*sizeof(QP_STORE_T));
    reallocAlign((void **)&c->stdQPTable, qpStride*mbHeight*sizeof(QP_STORE_T));
    reallocAlign((void **)&c->forcedQPTable, mbWidth*sizeof(QP_STORE_T));
}
