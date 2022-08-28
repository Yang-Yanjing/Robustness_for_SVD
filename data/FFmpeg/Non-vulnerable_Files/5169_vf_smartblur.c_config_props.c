static int config_props(AVFilterLink *inlink)
{
    SmartblurContext *s = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    alloc_sws_context(&s->luma, inlink->w, inlink->h, s->sws_flags);
    alloc_sws_context(&s->chroma,
                      FF_CEIL_RSHIFT(inlink->w, s->hsub),
                      FF_CEIL_RSHIFT(inlink->h, s->vsub),
                      s->sws_flags);
    return 0;
}
