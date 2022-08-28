static int config_props(AVFilterLink *link)
{
    UnsharpContext *unsharp = link->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(link->format);
    int ret;
    unsharp->hsub = desc->log2_chroma_w;
    unsharp->vsub = desc->log2_chroma_h;
    ret = init_filter_param(link->dst, &unsharp->luma,   "luma",   link->w);
    if (ret < 0)
        return ret;
    ret = init_filter_param(link->dst, &unsharp->chroma, "chroma", FF_CEIL_RSHIFT(link->w, unsharp->hsub));
    if (ret < 0)
        return ret;
    return 0;
}
