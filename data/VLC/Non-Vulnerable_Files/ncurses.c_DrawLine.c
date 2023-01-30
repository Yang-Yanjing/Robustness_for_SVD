}
static void DrawLine(int y, int x, int w)
{
    if (w <= 0) return;
    attrset(A_REVERSE);
    mvhline(y, x, ' ', w);
    attroff(A_REVERSE);
}
