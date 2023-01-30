}
static void DrawNumber(plane_t *p, int h, const uint8_t data[5], int l)
{
    for (int i = 0; i < 5; i++) {
        uint8_t x = data[i];
        for (int j = 0; j < 7; j++) {
            x <<= 1;
            if (x & 0x80)
                DrawHLine(p, h - l + 2 - 1 - i, 12 + j, black, 1);
        }
    }
}
