static av_always_inline void
yuv2planeX_10_c_template(const int16_t *filter, int filterSize,
                         const int16_t **src, uint16_t *dest, int dstW,
                         int big_endian, int output_bits)
{
    int i;
    int shift = 11 + 16 - output_bits;
    for (i = 0; i < dstW; i++) {
        int val = 1 << (shift - 1);
        int j;
        for (j = 0; j < filterSize; j++)
            val += src[j][i] * filter[j];
        output_pixel(&dest[i], val);
    }
}
