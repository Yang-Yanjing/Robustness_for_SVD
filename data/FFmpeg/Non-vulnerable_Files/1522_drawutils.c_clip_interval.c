static void clip_interval(int wmax, int *x, int *w, int *dx)
{
    if (dx)
        *dx = 0;
    if (*x < 0) {
        if (dx)
            *dx = -*x;
        *w += *x;
        *x = 0;
    }
    if (*x + *w > wmax)
        *w = wmax - *x;
}
