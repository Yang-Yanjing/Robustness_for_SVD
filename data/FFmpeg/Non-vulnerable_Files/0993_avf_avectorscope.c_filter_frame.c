static int filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    AudioVectorScopeContext *s = ctx->priv;
    const int hw = s->hw;
    const int hh = s->hh;
    unsigned x, y;
    const double zoom = s->zoom;
    int i;
    if (!s->outpicref || s->outpicref->width  != outlink->w ||
                         s->outpicref->height != outlink->h) {
        av_frame_free(&s->outpicref);
        s->outpicref = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!s->outpicref) {
            av_frame_free(&insamples);
            return AVERROR(ENOMEM);
        }
        for (i = 0; i < outlink->h; i++)
            memset(s->outpicref->data[0] + i * s->outpicref->linesize[0], 0, outlink->w * 4);
    }
    s->outpicref->pts = insamples->pts;
    fade(s);
    switch (insamples->format) {
    case AV_SAMPLE_FMT_S16:
        for (i = 0; i < insamples->nb_samples; i++) {
            int16_t *src = (int16_t *)insamples->data[0] + i * 2;
            if (s->mode == LISSAJOUS) {
                x = ((src[1] - src[0]) * zoom / (float)(UINT16_MAX) + 1) * hw;
                y = (1.0 - (src[0] + src[1]) * zoom / (float)UINT16_MAX) * hh;
            } else if (s->mode == LISSAJOUS_XY) {
                x = (src[1] * zoom / (float)INT16_MAX + 1) * hw;
                y = (src[0] * zoom / (float)INT16_MAX + 1) * hh;
            } else {
                float sx, sy, cx, cy;
                sx = src[1] * zoom / (float)INT16_MAX;
                sy = src[0] * zoom / (float)INT16_MAX;
                cx = sx * sqrtf(1 - 0.5*sy*sy);
                cy = sy * sqrtf(1 - 0.5*sx*sx);
                x = hw + hw * FFSIGN(cx + cy) * (cx - cy) * .7;
                y = s->h - s->h * FFABS(cx + cy) * .7;
            }
            draw_dot(s, x, y);
        }
        break;
    case AV_SAMPLE_FMT_FLT:
        for (i = 0; i < insamples->nb_samples; i++) {
            float *src = (float *)insamples->data[0] + i * 2;
            if (s->mode == LISSAJOUS) {
                x = ((src[1] - src[0]) * zoom / 2 + 1) * hw;
                y = (1.0 - (src[0] + src[1]) * zoom / 2) * hh;
            } else if (s->mode == LISSAJOUS_XY){
                x = (src[1] * zoom + 1) * hw;
                y = (src[0] * zoom + 1) * hh;
            } else {
                float sx, sy, cx, cy;
                sx = src[1] * zoom;
                sy = src[0] * zoom;
                cx = sx * sqrtf(1 - 0.5 * sy * sy);
                cy = sy * sqrtf(1 - 0.5 * sx * sx);
                x = hw + hw * FFSIGN(cx + cy) * (cx - cy) * .7;
                y = s->h - s->h * FFABS(cx + cy) * .7;
            }
            draw_dot(s, x, y);
        }
        break;
    }
    av_frame_free(&insamples);
    return ff_filter_frame(outlink, av_frame_clone(s->outpicref));
}
