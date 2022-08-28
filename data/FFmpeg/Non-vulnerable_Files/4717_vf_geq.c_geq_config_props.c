static int geq_config_props(AVFilterLink *inlink)
{
    GEQContext *geq = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    av_assert0(desc);
    geq->hsub = desc->log2_chroma_w;
    geq->vsub = desc->log2_chroma_h;
    geq->planes = desc->nb_components;
    return 0;
}
