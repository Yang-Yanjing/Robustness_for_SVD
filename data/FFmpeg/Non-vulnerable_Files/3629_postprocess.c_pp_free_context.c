av_cold void pp_free_context(void *vc){
    PPContext *c = (PPContext*)vc;
    int i;
    for(i=0; i<FF_ARRAY_ELEMS(c->tempBlurred); i++)
        av_free(c->tempBlurred[i]);
    for(i=0; i<FF_ARRAY_ELEMS(c->tempBlurredPast); i++)
        av_free(c->tempBlurredPast[i]);
    av_free(c->tempBlocks);
    av_free(c->yHistogram);
    av_free(c->tempDst);
    av_free(c->tempSrc);
    av_free(c->deintTemp);
    av_free(c->stdQPTable);
    av_free(c->nonBQPTable);
    av_free(c->forcedQPTable);
    memset(c, 0, sizeof(PPContext));
    av_free(c);
}
