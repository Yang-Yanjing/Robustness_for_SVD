static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    ExtractPlanesContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    const int output = outlink->srcpad - ctx->output_pads;
    if (s->map[output] == 1 || s->map[output] == 2) {
        outlink->h = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
        outlink->w = FF_CEIL_RSHIFT(inlink->w, desc->log2_chroma_w);
    }
    return 0;
}
