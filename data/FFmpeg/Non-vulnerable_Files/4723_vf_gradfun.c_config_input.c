static int config_input(AVFilterLink *inlink)
{
    GradFunContext *s = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int hsub = desc->log2_chroma_w;
    int vsub = desc->log2_chroma_h;
    av_freep(&s->buf);
    s->buf = av_calloc((FFALIGN(inlink->w, 16) * (s->radius + 1) / 2 + 32), sizeof(*s->buf));
    if (!s->buf)
        return AVERROR(ENOMEM);
    s->chroma_w = FF_CEIL_RSHIFT(inlink->w, hsub);
    s->chroma_h = FF_CEIL_RSHIFT(inlink->h, vsub);
    s->chroma_r = av_clip(((((s->radius >> hsub) + (s->radius >> vsub)) / 2 ) + 1) & ~1, 4, 32);
    return 0;
}
