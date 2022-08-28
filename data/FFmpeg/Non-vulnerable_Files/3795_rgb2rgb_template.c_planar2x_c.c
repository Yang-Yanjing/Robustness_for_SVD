static inline void planar2x_c(const uint8_t *src, uint8_t *dst, int srcWidth,
                              int srcHeight, int srcStride, int dstStride)
{
    int x, y;
    dst[0] = src[0];
    
    for (x = 0; x < srcWidth - 1; x++) {
        dst[2 * x + 1] = (3 * src[x] + src[x + 1]) >> 2;
        dst[2 * x + 2] = (src[x] + 3 * src[x + 1]) >> 2;
    }
    dst[2 * srcWidth - 1] = src[srcWidth - 1];
    dst += dstStride;
    for (y = 1; y < srcHeight; y++) {
        const int mmxSize = 1;
        dst[0]         = (src[0] * 3 + src[srcStride]) >> 2;
        dst[dstStride] = (src[0] + 3 * src[srcStride]) >> 2;
        for (x = mmxSize - 1; x < srcWidth - 1; x++) {
            dst[2 * x + 1]             = (src[x + 0] * 3 + src[x + srcStride + 1]) >> 2;
            dst[2 * x + dstStride + 2] = (src[x + 0] + 3 * src[x + srcStride + 1]) >> 2;
            dst[2 * x + dstStride + 1] = (src[x + 1] + 3 * src[x + srcStride])     >> 2;
            dst[2 * x + 2]             = (src[x + 1] * 3 + src[x + srcStride])     >> 2;
        }
        dst[srcWidth * 2 - 1]             = (src[srcWidth - 1] * 3 + src[srcWidth - 1 + srcStride]) >> 2;
        dst[srcWidth * 2 - 1 + dstStride] = (src[srcWidth - 1] + 3 * src[srcWidth - 1 + srcStride]) >> 2;
        dst += dstStride * 2;
        src += srcStride;
    }
    
    dst[0] = src[0];
    for (x = 0; x < srcWidth - 1; x++) {
        dst[2 * x + 1] = (src[x] * 3 + src[x + 1]) >> 2;
        dst[2 * x + 2] = (src[x] + 3 * src[x + 1]) >> 2;
    }
    dst[2 * srcWidth - 1] = src[srcWidth - 1];
}
