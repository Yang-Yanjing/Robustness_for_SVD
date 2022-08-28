static void BAYER_RENAME(rgb24_interpolate)(const uint8_t *src, int src_stride, uint8_t *dst, int dst_stride, int width)
{
    int i;
    BAYER_TO_RGB24_COPY
    src += 2 * BAYER_SIZEOF;
    dst += 6;
    for (i = 2 ; i < width - 2; i+= 2) {
        BAYER_TO_RGB24_INTERPOLATE
        src += 2 * BAYER_SIZEOF;
        dst += 6;
    }
    if (width > 2) {
        BAYER_TO_RGB24_COPY
    }
}
