static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    FrameRateContext *s = ctx->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(inlink->format);
    int plane;
    for (plane = 0; plane < 4; plane++) {
        s->line_size[plane] = av_image_get_linesize(inlink->format, inlink->w,
                                                    plane);
    }
    s->vsub = pix_desc->log2_chroma_h;
    s->sad = av_pixelutils_get_sad_fn(3, 3, 2, s); 
    if (!s->sad)
        return AVERROR(EINVAL);
    s->srce_time_base = inlink->time_base;
    return 0;
}
