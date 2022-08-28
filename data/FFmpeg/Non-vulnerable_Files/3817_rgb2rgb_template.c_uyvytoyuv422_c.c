static void uyvytoyuv422_c(uint8_t *ydst, uint8_t *udst, uint8_t *vdst,
                           const uint8_t *src, int width, int height,
                           int lumStride, int chromStride, int srcStride)
{
    int y;
    const int chromWidth = FF_CEIL_RSHIFT(width, 1);
    for (y = 0; y < height; y++) {
        extract_even_c(src + 1, ydst, width);
        extract_even2_c(src, udst, vdst, chromWidth);
        src  += srcStride;
        ydst += lumStride;
        udst += chromStride;
        vdst += chromStride;
    }
}
}
static void uyvytoyuv422_c(uint8_t *ydst, uint8_t *udst, uint8_t *vdst,
                           const uint8_t *src, int width, int height,
                           int lumStride, int chromStride, int srcStride)
{
    int y;
    const int chromWidth = FF_CEIL_RSHIFT(width, 1);
    for (y = 0; y < height; y++) {
        extract_even_c(src + 1, ydst, width);
        extract_even2_c(src, udst, vdst, chromWidth);
        src  += srcStride;
        ydst += lumStride;
        udst += chromStride;
        vdst += chromStride;
    }
