static void chrRangeToJpeg16_c(int16_t *_dstU, int16_t *_dstV, int width)
{
    int i;
    int32_t *dstU = (int32_t *) _dstU;
    int32_t *dstV = (int32_t *) _dstV;
    for (i = 0; i < width; i++) {
        dstU[i] = (FFMIN(dstU[i], 30775 << 4) * 4663 - (9289992 << 4)) >> 12; 
        dstV[i] = (FFMIN(dstV[i], 30775 << 4) * 4663 - (9289992 << 4)) >> 12; 
    }
}
