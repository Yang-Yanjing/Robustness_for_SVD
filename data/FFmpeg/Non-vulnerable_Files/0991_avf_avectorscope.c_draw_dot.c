AVFILTER_DEFINE_CLASS(avectorscope);
static void draw_dot(AudioVectorScopeContext *s, unsigned x, unsigned y)
{
    const int linesize = s->outpicref->linesize[0];
    uint8_t *dst;
    if (s->zoom > 1) {
        if (y >= s->h || x >= s->w)
            return;
    } else {
        y = FFMIN(y, s->h - 1);
        x = FFMIN(x, s->w - 1);
    }
    dst = &s->outpicref->data[0][y * linesize + x * 4];
    dst[0] = FFMIN(dst[0] + s->contrast[0], 255);
    dst[1] = FFMIN(dst[1] + s->contrast[1], 255);
    dst[2] = FFMIN(dst[2] + s->contrast[2], 255);
    dst[3] = FFMIN(dst[3] + s->contrast[3], 255);
}
