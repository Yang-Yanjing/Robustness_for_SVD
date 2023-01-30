static int config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    MPTestContext *test = ctx->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(outlink->format);
    test->hsub = pix_desc->log2_chroma_w;
    test->vsub = pix_desc->log2_chroma_h;
    outlink->w = WIDTH;
    outlink->h = HEIGHT;
    outlink->time_base = av_inv_q(test->frame_rate);
    return 0;
}
