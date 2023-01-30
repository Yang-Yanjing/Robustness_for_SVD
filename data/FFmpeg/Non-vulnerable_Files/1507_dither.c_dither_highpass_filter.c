static void dither_highpass_filter(float *src, int len)
{
    int i;
    
    for (i = 0; i < len - 2; i++)
        src[i] = (-src[i] + 2 * src[i + 1] - src[i + 2]) * SQRT_1_6;
}
