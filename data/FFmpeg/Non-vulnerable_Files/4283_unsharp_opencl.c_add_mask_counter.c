static inline void add_mask_counter(uint32_t *dst, uint32_t *counter1, uint32_t *counter2, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        dst[i] = counter1[i] + counter2[i];
    }
}
