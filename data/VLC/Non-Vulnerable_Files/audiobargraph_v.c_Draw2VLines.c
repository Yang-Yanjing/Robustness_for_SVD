}
static void Draw2VLines(plane_t *p, int scale, int col, const uint8_t color[4])
{
    for (int i = 10; i < scale + 10; i++)
        DrawHLine(p, i, col, color, 2);
}
