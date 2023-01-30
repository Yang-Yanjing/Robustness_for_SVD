static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    HisteqContext *histeq = ctx->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(inlink->format);
    histeq->bpp = av_get_bits_per_pixel(pix_desc) / 8;
    ff_fill_rgba_map(histeq->rgba_map, inlink->format);
    return 0;
}
