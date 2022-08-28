static inline void plot_freq(ShowFreqsContext *s, int ch,
                             double a, int f, uint8_t fg[4], int *prev_y,
                             AVFrame *out, AVFilterLink *outlink)
{
    const int w = s->w;
    const float avg = s->avg_data[ch][f];
    const float bsize = get_bsize(s, f);
    const int sx = get_sx(s, f);
    int x, y, i;
    switch(s->ascale) {
    case AS_SQRT:
        a = 1.0 - sqrt(a);
        break;
    case AS_CBRT:
        a = 1.0 - cbrt(a);
        break;
    case AS_LOG:
        a = log(av_clipd(a, 1e-6, 1)) / log(1e-6);
        break;
    case AS_LINEAR:
        a = 1.0 - a;
        break;
    }
    y = a * outlink->h - 1;
    if (y < 0)
        return;
    switch (s->avg) {
    case 0:
        y = s->avg_data[ch][f] = !outlink->frame_count ? y : FFMIN(avg, y);
        break;
    case 1:
        break;
    default:
        s->avg_data[ch][f] = avg + y * (y - avg) / (FFMIN(outlink->frame_count + 1, s->avg) * y);
        y = s->avg_data[ch][f];
        break;
    }
    switch(s->mode) {
    case LINE:
        if (*prev_y == -1) {
            *prev_y = y;
        }
        if (y <= *prev_y) {
            for (x = sx + 1; x < sx + bsize && x < w; x++)
                draw_dot(out, x, y, fg);
            for (i = y; i <= *prev_y; i++)
                draw_dot(out, sx, i, fg);
        } else {
            for (i = *prev_y; i <= y; i++)
                draw_dot(out, sx, i, fg);
            for (x = sx + 1; x < sx + bsize && x < w; x++)
                draw_dot(out, x, i - 1, fg);
        }
        *prev_y = y;
        break;
    case BAR:
        for (x = sx; x < sx + bsize && x < w; x++)
            for (i = y; i < outlink->h; i++)
                draw_dot(out, x, i, fg);
        break;
    case DOT:
        for (x = sx; x < sx + bsize && x < w; x++)
            draw_dot(out, x, y, fg);
        break;
    }
}
