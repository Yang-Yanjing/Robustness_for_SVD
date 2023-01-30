static void quantize_c(int16_t *dst, const float *src, float *dither, int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[i] = av_clip_int16(lrintf(src[i] * S16_SCALE + dither[i]));
}
