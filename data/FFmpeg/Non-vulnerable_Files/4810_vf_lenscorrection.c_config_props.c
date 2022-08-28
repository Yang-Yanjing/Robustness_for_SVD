static int config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    LenscorrectionCtx *rect = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    const AVPixFmtDescriptor *pixdesc = av_pix_fmt_desc_get(inlink->format);
    rect->hsub = pixdesc->log2_chroma_w;
    rect->vsub = pixdesc->log2_chroma_h;
    outlink->w = rect->width = inlink->w;
    outlink->h = rect->height = inlink->h;
    rect->nb_planes = av_pix_fmt_count_planes(inlink->format);
    return 0;
}
