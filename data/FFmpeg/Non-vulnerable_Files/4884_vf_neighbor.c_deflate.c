static void deflate(uint8_t *dst, const uint8_t *p1, int width,
                    int threshold, const uint8_t *coordinates[], int coord)
{
    int x, i;
    for (x = 0; x < width; x++) {
        int sum = 0;
        int limit = FFMAX(p1[x] - threshold, 0);
        for (i = 0; i < 8; sum += *(coordinates[i++] + x));
        dst[x] = FFMAX(FFMIN(sum / 8, p1[x]), limit);
    }
}
