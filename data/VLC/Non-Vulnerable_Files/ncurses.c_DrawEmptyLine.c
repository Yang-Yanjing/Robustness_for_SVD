}
static void DrawEmptyLine(int y, int x, int w)
{
    if (w <= 0) return;
    mvhline(y, x, ' ', w);
}
