static void draw_arrow(uint8_t *buf, int sx, int sy, int ex,
                       int ey, int w, int h, int stride, int color, int tail, int direction)
{
    int dx,dy;
    if (direction) {
        FFSWAP(int, sx, ex);
        FFSWAP(int, sy, ey);
    }
    sx = av_clip(sx, -100, w + 100);
    sy = av_clip(sy, -100, h + 100);
    ex = av_clip(ex, -100, w + 100);
    ey = av_clip(ey, -100, h + 100);
    dx = ex - sx;
    dy = ey - sy;
    if (dx * dx + dy * dy > 3 * 3) {
        int rx =  dx + dy;
        int ry = -dx + dy;
        int length = sqrt((rx * rx + ry * ry) << 8);
        
        rx = ROUNDED_DIV(rx * 3 << 4, length);
        ry = ROUNDED_DIV(ry * 3 << 4, length);
        if (tail) {
            rx = -rx;
            ry = -ry;
        }
        draw_line(buf, sx, sy, sx + rx, sy + ry, w, h, stride, color);
        draw_line(buf, sx, sy, sx - ry, sy + rx, w, h, stride, color);
    }
    draw_line(buf, sx, sy, ex, ey, w, h, stride, color);
}
