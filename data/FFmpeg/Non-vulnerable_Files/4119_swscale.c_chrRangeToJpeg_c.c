
static void chrRangeToJpeg_c(int16_t *dstU, int16_t *dstV, int width)
{
    int i;
    for (i = 0; i < width; i++) {
        dstU[i] = (FFMIN(dstU[i], 30775) * 4663 - 9289992) >> 12; 
        dstV[i] = (FFMIN(dstV[i], 30775) * 4663 - 9289992) >> 12; 
    }
}
