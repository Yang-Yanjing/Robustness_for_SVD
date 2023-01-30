static void chrRangeFromJpeg_c(int16_t *dstU, int16_t *dstV, int width)
{
    int i;
    for (i = 0; i < width; i++) {
        dstU[i] = (dstU[i] * 1799 + 4081085) >> 11; 
        dstV[i] = (dstV[i] * 1799 + 4081085) >> 11; 
    }
}
