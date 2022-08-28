static void compute_metric(PullupContext *s, int *dest,
                           PullupField *fa, int pa, PullupField *fb, int pb,
                           int (*func)(const uint8_t *, const uint8_t *, ptrdiff_t))
{
    int mp = s->metric_plane;
    int xstep = 8;
    int ystep = s->planewidth[mp] << 3;
    int stride = s->planewidth[mp] << 1; 
    int w = s->metric_w * xstep;
    uint8_t *a, *b;
    int x, y;
    if (!fa->buffer || !fb->buffer)
        return;
    
    if (fa->buffer == fb->buffer && pa == pb) {
        memset(dest, 0, s->metric_length * sizeof(*dest));
        return;
    }
    a = fa->buffer->planes[mp] + pa * s->planewidth[mp] + s->metric_offset;
    b = fb->buffer->planes[mp] + pb * s->planewidth[mp] + s->metric_offset;
    for (y = 0; y < s->metric_h; y++) {
        for (x = 0; x < w; x += xstep)
            *dest++ = func(a + x, b + x, stride);
        a += ystep; b += ystep;
    }
}
