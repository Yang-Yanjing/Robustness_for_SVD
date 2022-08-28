static int config_input(AVFilterLink *link)
{
    FlipContext *flip = link->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(link->format);
    flip->vsub = desc->log2_chroma_h;
    return 0;
}
