}
static void DrawBox(int y, int h, bool color, const char *title)
{
    int w = COLS;
    if (w <= 3 || h <= 0)
        return;
    if (color) color_set(C_BOX, NULL);
    if (!title) title = "";
    int len = strlen(title);
    if (len > w - 2)
        len = w - 2;
    mvaddch(y, 0,    ACS_ULCORNER);
    mvhline(y, 1,  ACS_HLINE, (w-len-2)/2);
    mvprintw(y, 1+(w-len-2)/2, "%s", title);
    mvhline(y, (w-len)/2+len,  ACS_HLINE, w - 1 - ((w-len)/2+len));
    mvaddch(y, w-1,ACS_URCORNER);
    for (int i = 0; i < h; i++) {
        mvaddch(++y, 0,   ACS_VLINE);
        mvaddch(y, w-1, ACS_VLINE);
    }
    mvaddch(++y, 0,   ACS_LLCORNER);
    mvhline(y,   1,   ACS_HLINE, w - 2);
    mvaddch(y,   w-1, ACS_LRCORNER);
    if (color) color_set(C_DEFAULT, NULL);
}
