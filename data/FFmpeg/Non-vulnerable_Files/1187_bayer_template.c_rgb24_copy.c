    ff_rgb24toyv12(src, dstY, dstV, dstU, 2, 2, luma_stride, 0, src_stride, rgb2yuv)
static void BAYER_RENAME(rgb24_copy)(const uint8_t *src, int src_stride, uint8_t *dst, int dst_stride, int width)
{
    int i;
    for (i = 0 ; i < width; i+= 2) {
        BAYER_TO_RGB24_COPY
        src += 2 * BAYER_SIZEOF;
        dst += 6;
    }
}
