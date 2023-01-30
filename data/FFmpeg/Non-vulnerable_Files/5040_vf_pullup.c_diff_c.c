static int diff_c(const uint8_t *a, const uint8_t *b, ptrdiff_t s)
{
    int i, j, diff = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++)
            diff += ABS(a[j] - b[j]);
        a += s;
        b += s;
    }
    return diff;
}
