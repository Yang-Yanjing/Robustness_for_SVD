static void gen_image(int num, int w, int h)
{
    const int c = h_cos[num % 360];
    const int s = h_sin[num % 360];
    const int xi = -(w / 2) * c;
    const int yi =  (w / 2) * s;
    const int xj = -(h / 2) * s;
    const int yj = -(h / 2) * c;
    int i, j;
    int x, y;
    int xprime = xj;
    int yprime = yj;
    for (j = 0; j < h; j++) {
        x       = xprime + xi + FIXP * w / 2;
        xprime += s;
        y       = yprime + yi + FIXP * h / 2;
        yprime += c;
        for (i = 0; i < w; i++) {
            x += c;
            y -= s;
            put_pixel(i, j,
                      ipol(tab_r, x, y),
                      ipol(tab_g, x, y),
                      ipol(tab_b, x, y));
        }
    }
}
