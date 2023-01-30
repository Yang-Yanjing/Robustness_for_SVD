static void BAYER_RENAME(yv12_copy)(const uint8_t *src, int src_stride, uint8_t *dstY, uint8_t *dstU, uint8_t *dstV, int luma_stride, int width, int32_t *rgb2yuv)
{
    uint8_t dst[12];
    const int dst_stride = 6;
    int i;
    for (i = 0 ; i < width; i+= 2) {
        BAYER_TO_RGB24_COPY
        rgb24toyv12_2x2(dst, dstY, dstU, dstV, luma_stride, dst_stride, rgb2yuv);
        src  += 2 * BAYER_SIZEOF;
        dstY += 2;
        dstU++;
        dstV++;
    }
}
