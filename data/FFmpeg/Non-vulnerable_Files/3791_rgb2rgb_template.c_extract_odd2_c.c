static void extract_odd2_c(const uint8_t *src, uint8_t *dst0, uint8_t *dst1,
                           int count)
{
    dst0  +=  count;
    dst1  +=  count;
    src   +=  count * 4;
    count  = -count;
    src++;
    while (count < 0) {
        dst0[count] = src[4 * count + 0];
        dst1[count] = src[4 * count + 2];
        count++;
    }
}
