static void update_sample_stats(const uint8_t *src, int len, int64_t *sum, int64_t *sum2)
{
    int i;
    for (i = 0; i < len; i++) {
        *sum += src[i];
        *sum2 += src[i] * src[i];
    }
}
