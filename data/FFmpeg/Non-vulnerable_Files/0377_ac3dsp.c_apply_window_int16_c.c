static void apply_window_int16_c(int16_t *output, const int16_t *input,
                                 const int16_t *window, unsigned int len)
{
    int i;
    int len2 = len >> 1;
    for (i = 0; i < len2; i++) {
        int16_t w       = window[i];
        output[i]       = (MUL16(input[i],       w) + (1 << 14)) >> 15;
        output[len-i-1] = (MUL16(input[len-i-1], w) + (1 << 14)) >> 15;
    }
}
