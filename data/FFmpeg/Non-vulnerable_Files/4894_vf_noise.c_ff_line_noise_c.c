void ff_line_noise_c(uint8_t *dst, const uint8_t *src, const int8_t *noise,
                     int len, int shift)
{
    int i;
    noise += shift;
    for (i = 0; i < len; i++) {
        int v = src[i] + noise[i];
        dst[i] = av_clip_uint8(v);
    }
}
