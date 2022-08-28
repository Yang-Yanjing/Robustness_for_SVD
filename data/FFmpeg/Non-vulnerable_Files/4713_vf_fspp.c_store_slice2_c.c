
static void store_slice2_c(uint8_t *dst, int16_t *src,
                           ptrdiff_t dst_stride, ptrdiff_t src_stride,
                           ptrdiff_t width, ptrdiff_t height, ptrdiff_t log2_scale)
{
    int y, x;
#define STORE2(pos)                                                                                       \
    temp = (src[x + pos] + src[x + pos + 16 * src_stride] + (d[pos] >> log2_scale)) >> (6 - log2_scale);  \
    src[x + pos + 16 * src_stride] = 0;                                                                   \
    if (temp & 0x100) temp = ~(temp >> 31);                                                               \
    dst[x + pos] = temp;
    for (y = 0; y < height; y++) {
        const uint8_t *d = dither[y];
        for (x = 0; x < width; x += 8) {
            int temp;
            STORE2(0);
            STORE2(1);
            STORE2(2);
            STORE2(3);
            STORE2(4);
            STORE2(5);
            STORE2(6);
            STORE2(7);
        }
        src += src_stride;
        dst += dst_stride;
    }
}
