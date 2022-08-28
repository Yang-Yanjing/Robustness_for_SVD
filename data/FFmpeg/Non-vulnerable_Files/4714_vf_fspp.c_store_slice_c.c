
static void store_slice_c(uint8_t *dst, int16_t *src,
                          ptrdiff_t dst_stride, ptrdiff_t src_stride,
                          ptrdiff_t width, ptrdiff_t height, ptrdiff_t log2_scale)
{
    int y, x;
#define STORE(pos)                                                             \
    temp = (src[x + pos] + (d[pos] >> log2_scale)) >> (6 - log2_scale);        \
    src[x + pos] = src[x + pos - 8 * src_stride] = 0;                          \
    if (temp & 0x100) temp = ~(temp >> 31);                                    \
    dst[x + pos] = temp;
    for (y = 0; y < height; y++) {
        const uint8_t *d = dither[y];
        for (x = 0; x < width; x += 8) {
            int temp;
            STORE(0);
            STORE(1);
            STORE(2);
            STORE(3);
            STORE(4);
            STORE(5);
            STORE(6);
            STORE(7);
        }
        src += src_stride;
        dst += dst_stride;
    }
}
