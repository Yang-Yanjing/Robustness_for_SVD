static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    WaveformContext *s = ctx->priv;
    int comp = 0, i, j = 0, k, p, size, shift;
    for (i = 0; i < s->ncomp; i++) {
        if ((1 << i) & s->pcomp)
            comp++;
    }
    av_freep(&s->peak);
    if (s->mode) {
        outlink->h = s->size * FFMAX(comp * s->display, 1);
        size = inlink->w;
    } else {
        outlink->w = s->size * FFMAX(comp * s->display, 1);
        size = inlink->h;
    }
    s->peak = av_malloc_array(size, 32 * sizeof(*s->peak));
    if (!s->peak)
        return AVERROR(ENOMEM);
    for (p = 0; p < 4; p++) {
        const int is_chroma = (p == 1 || p == 2);
        const int shift_w = (is_chroma ? s->desc->log2_chroma_w : 0);
        const int shift_h = (is_chroma ? s->desc->log2_chroma_h : 0);
        const int plane = s->desc->comp[p].plane;
        int offset;
        if (!((1 << p) & s->pcomp))
            continue;
        shift = s->mode ? shift_h : shift_w;
        for (k = 0; k < 4; k++) {
            s->emax[plane][k] = s->peak + size * (plane * 4 + k + 0);
            s->emin[plane][k] = s->peak + size * (plane * 4 + k + 16);
        }
        offset = j++ * s->size * s->display;
        s->estart[plane] = offset >> shift;
        s->eend[plane]   = (offset + s->size - 1) >> shift;
        for (i = 0; i < size; i++) {
            for (k = 0; k < 4; k++) {
                s->emax[plane][k][i] = s->estart[plane];
                s->emin[plane][k][i] = s->eend[plane];
            }
        }
    }
    outlink->sample_aspect_ratio = (AVRational){1,1};
    return 0;
}
