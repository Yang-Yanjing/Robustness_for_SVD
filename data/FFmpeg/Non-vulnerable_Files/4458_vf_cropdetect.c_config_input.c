static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    CropDetectContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    av_image_fill_max_pixsteps(s->max_pixsteps, NULL, desc);
    if (s->limit < 1.0)
        s->limit *= (1 << (desc->comp[0].depth_minus1 + 1)) - 1;
    s->x1 = inlink->w - 1;
    s->y1 = inlink->h - 1;
    s->x2 = 0;
    s->y2 = 0;
    return 0;
}
