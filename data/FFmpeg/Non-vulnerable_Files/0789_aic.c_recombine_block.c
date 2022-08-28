static void recombine_block(int16_t *dst, const uint8_t *scan,
                            int16_t **base, int16_t **ext)
{
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++)
            dst[scan[i * 8 + j]]     = (*base)[j];
        for (j = 0; j < 4; j++)
            dst[scan[i * 8 + j + 4]] = (*ext)[j];
        *base += 4;
        *ext  += 4;
    }
    for (; i < 8; i++) {
        for (j = 0; j < 8; j++)
            dst[scan[i * 8 + j]] = (*ext)[j];
        *ext  += 8;
    }
}
