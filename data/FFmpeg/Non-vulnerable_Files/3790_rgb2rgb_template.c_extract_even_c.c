static void extract_even_c(const uint8_t *src, uint8_t *dst, int count)
{
    dst   +=  count;
    src   +=  count * 2;
    count  = -count;
    while (count < 0) {
        dst[count] = src[2 * count];
        count++;
    }
}
