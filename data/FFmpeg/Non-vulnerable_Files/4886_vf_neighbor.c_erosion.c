static void erosion(uint8_t *dst, const uint8_t *p1, int width,
                    int threshold, const uint8_t *coordinates[], int coord)
{
    int x, i;
    for (x = 0; x < width; x++) {
        int min = p1[x];
        int limit = FFMAX(min - threshold, 0);
        for (i = 0; i < 8; i++) {
            if (coord & (1 << i)) {
                min = FFMIN(min, *(coordinates[i] + x));
            }
            min = FFMAX(min, limit);
        }
        dst[x] = min;
    }
}
