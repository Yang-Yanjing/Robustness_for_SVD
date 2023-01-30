}
static void DrawHLines(plane_t *p, int line, int col, const uint8_t color[4], int h, int w)
{
    for (int i = line; i < line + h; i++)
        DrawHLine(p, i, col, color, w);
}
