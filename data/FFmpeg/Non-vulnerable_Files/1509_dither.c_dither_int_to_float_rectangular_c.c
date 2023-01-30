static void dither_int_to_float_rectangular_c(float *dst, int *src, int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[i] = src[i] * LFG_SCALE;
}
