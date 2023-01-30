static inline void yuv2planeX_u(const int16_t *filter, int filterSize,
                                const int16_t **src, uint8_t *dest, int dstW,
                                const uint8_t *dither, int offset, int x)
{
    int i, j;
    for (i = x; i < dstW; i++) {
        int t = dither[(i + offset) & 7] << 12;
        for (j = 0; j < filterSize; j++)
            t += src[j][i] * filter[j];
        dest[i] = av_clip_uint8(t >> 19);
    }
}
