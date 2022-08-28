static int comb_c(const uint8_t *a, const uint8_t *b, ptrdiff_t s)
{
    int i, j, comb = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++)
            comb += ABS((a[j] << 1) - b[j - s] - b[j    ]) +
                    ABS((b[j] << 1) - a[j    ] - a[j + s]);
        a += s;
        b += s;
    }
    return comb;
}
