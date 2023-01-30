int ff_rotate_slice(SwsSlice *s, int lum, int chr)
{
    int i;
    if (lum) {
        for (i = 0; i < 4; i+=3) {
            int n = s->plane[i].available_lines;
            int l = lum - s->plane[i].sliceY;
            if (l >= n * 2) {
                s->plane[i].sliceY += n;
                s->plane[i].sliceH -= n;
            }
        }
    }
    if (chr) {
        for (i = 1; i < 3; ++i) {
            int n = s->plane[i].available_lines;
            int l = chr - s->plane[i].sliceY;
            if (l >= n * 2) {
                s->plane[i].sliceY += n;
                s->plane[i].sliceH -= n;
            }
        }
    }
    return 0;
}
