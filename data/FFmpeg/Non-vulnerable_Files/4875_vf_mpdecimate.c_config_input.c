static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    DecimateContext *decimate = ctx->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(inlink->format);
    decimate->hsub = pix_desc->log2_chroma_w;
    decimate->vsub = pix_desc->log2_chroma_h;
    return 0;
}
