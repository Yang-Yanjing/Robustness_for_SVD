static void yuv2plane1_8_c(const int16_t *src, uint8_t *dest, int dstW,
                           const uint8_t *dither, int offset)
{
    int i;
    for (i=0; i<dstW; i++) {
        int val = (src[i] + dither[(i + offset) & 7]) >> 7;
        dest[i]= av_clip_uint8(val);
    }
}
