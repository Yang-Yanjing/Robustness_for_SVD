static void interleaveBytes_c(const uint8_t *src1, const uint8_t *src2,
                              uint8_t *dest, int width, int height,
                              int src1Stride, int src2Stride, int dstStride)
{
    int h;
    for (h = 0; h < height; h++) {
        int w;
        for (w = 0; w < width; w++) {
            dest[2 * w + 0] = src1[w];
            dest[2 * w + 1] = src2[w];
        }
        dest += dstStride;
        src1 += src1Stride;
        src2 += src2Stride;
    }
}
