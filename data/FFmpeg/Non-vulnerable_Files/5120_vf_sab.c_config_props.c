static int config_props(AVFilterLink *inlink)
{
    SabContext *s = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int ret;
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    close_filter_param(&s->luma);
    ret = open_filter_param(&s->luma, inlink->w, inlink->h, s->sws_flags);
    if (ret < 0)
        return ret;
    close_filter_param(&s->chroma);
    ret = open_filter_param(&s->chroma,
                            FF_CEIL_RSHIFT(inlink->w, s->hsub),
                            FF_CEIL_RSHIFT(inlink->h, s->vsub), s->sws_flags);
    return ret;
}
