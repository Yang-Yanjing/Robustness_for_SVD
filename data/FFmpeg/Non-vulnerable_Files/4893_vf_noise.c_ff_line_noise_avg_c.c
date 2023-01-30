void ff_line_noise_avg_c(uint8_t *dst, const uint8_t *src,
                         int len, const int8_t * const *shift)
{
    int i;
    const int8_t *src2 = (const int8_t*)src;
    for (i = 0; i < len; i++) {
        const int n = shift[0][i] + shift[1][i] + shift[2][i];
        dst[i] = src2[i] + ((n * src2[i]) >> 7);
    }
}
