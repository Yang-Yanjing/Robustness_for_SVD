static void ac3_lshift_int16_c(int16_t *src, unsigned int len,
                               unsigned int shift)
{
    uint32_t *src32 = (uint32_t *)src;
    const uint32_t mask = ~(((1 << shift) - 1) << 16);
    int i;
    len >>= 1;
    for (i = 0; i < len; i += 8) {
        src32[i  ] = (src32[i  ] << shift) & mask;
        src32[i+1] = (src32[i+1] << shift) & mask;
        src32[i+2] = (src32[i+2] << shift) & mask;
        src32[i+3] = (src32[i+3] << shift) & mask;
        src32[i+4] = (src32[i+4] << shift) & mask;
        src32[i+5] = (src32[i+5] << shift) & mask;
        src32[i+6] = (src32[i+6] << shift) & mask;
        src32[i+7] = (src32[i+7] << shift) & mask;
    }
}
