static void yuv2nv12cX_c(SwsContext *c, const int16_t *chrFilter, int chrFilterSize,
                        const int16_t **chrUSrc, const int16_t **chrVSrc,
                        uint8_t *dest, int chrDstW)
{
    enum AVPixelFormat dstFormat = c->dstFormat;
    const uint8_t *chrDither = c->chrDither8;
    int i;
    if (dstFormat == AV_PIX_FMT_NV12)
        for (i=0; i<chrDstW; i++) {
            int u = chrDither[i & 7] << 12;
            int v = chrDither[(i + 3) & 7] << 12;
            int j;
            for (j=0; j<chrFilterSize; j++) {
                u += chrUSrc[j][i] * chrFilter[j];
                v += chrVSrc[j][i] * chrFilter[j];
            }
            dest[2*i]= av_clip_uint8(u>>19);
            dest[2*i+1]= av_clip_uint8(v>>19);
        }
    else
        for (i=0; i<chrDstW; i++) {
            int u = chrDither[i & 7] << 12;
            int v = chrDither[(i + 3) & 7] << 12;
            int j;
            for (j=0; j<chrFilterSize; j++) {
                u += chrUSrc[j][i] * chrFilter[j];
                v += chrVSrc[j][i] * chrFilter[j];
            }
            dest[2*i]= av_clip_uint8(v>>19);
            dest[2*i+1]= av_clip_uint8(u>>19);
        }
}
