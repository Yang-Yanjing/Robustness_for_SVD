static void extract_even2avg_c(const uint8_t *src0, const uint8_t *src1,
                               uint8_t *dst0, uint8_t *dst1, int count)
{
    dst0  +=  count;
    dst1  +=  count;
    src0  +=  count * 4;
    src1  +=  count * 4;
    count  = -count;
    while (count < 0) {
        dst0[count] = (src0[4 * count + 0] + src1[4 * count + 0]) >> 1;
        dst1[count] = (src0[4 * count + 2] + src1[4 * count + 2]) >> 1;
        count++;
    }
}
