static void dilation(uint8_t *dst, const uint8_t *p1, int width,
                     int threshold, const uint8_t *coordinates[], int coord)
{
    int x, i;
    for (x = 0; x < width; x++) {
        int max = p1[x];
        int limit = FFMIN(max + threshold, 255);
        for (i = 0; i < 8; i++) {
            if (coord & (1 << i)) {
                max = FFMAX(max, *(coordinates[i] + x));
            }
            max = FFMIN(max, limit);
        }
        dst[x] = max;
    }
}
