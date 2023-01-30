static void lumRangeFromJpeg16_c(int16_t *_dst, int width)
{
    int i;
    int32_t *dst = (int32_t *) _dst;
    for (i = 0; i < width; i++)
        dst[i] = (dst[i]*(14071/4) + (33561947<<4)/4)>>12;
}
