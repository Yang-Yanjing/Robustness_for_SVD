static void hvcc_close(HEVCDecoderConfigurationRecord *hvcc)
{
    uint8_t i;
    for (i = 0; i < hvcc->numOfArrays; i++) {
        hvcc->array[i].numNalus = 0;
        av_freep(&hvcc->array[i].nalUnit);
        av_freep(&hvcc->array[i].nalUnitLength);
    }
    hvcc->numOfArrays = 0;
    av_freep(&hvcc->array);
}
