static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    ColorMatrixContext *color = ctx->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(inlink->format);
    color->hsub = pix_desc->log2_chroma_w;
    color->vsub = pix_desc->log2_chroma_h;
    av_log(ctx, AV_LOG_VERBOSE, "%s -> %s\n",
           color_modes[color->source], color_modes[color->dest]);
    return 0;
}
