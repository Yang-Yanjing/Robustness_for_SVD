static int config_clut(AVFilterLink *inlink)
{
    int size, level, w, h;
    AVFilterContext *ctx = inlink->dst;
    LUT3DContext *lut3d = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    av_assert0(desc);
    lut3d->clut_is16bit = 0;
    switch (inlink->format) {
    case AV_PIX_FMT_RGB48:
    case AV_PIX_FMT_BGR48:
    case AV_PIX_FMT_RGBA64:
    case AV_PIX_FMT_BGRA64:
        lut3d->clut_is16bit = 1;
    }
    lut3d->clut_step = av_get_padded_bits_per_pixel(desc) >> 3;
    ff_fill_rgba_map(lut3d->clut_rgba_map, inlink->format);
    if (inlink->w > inlink->h)
        av_log(ctx, AV_LOG_INFO, "Padding on the right (%dpx) of the "
               "Hald CLUT will be ignored\n", inlink->w - inlink->h);
    else if (inlink->w < inlink->h)
        av_log(ctx, AV_LOG_INFO, "Padding at the bottom (%dpx) of the "
               "Hald CLUT will be ignored\n", inlink->h - inlink->w);
    lut3d->clut_width = w = h = FFMIN(inlink->w, inlink->h);
    for (level = 1; level*level*level < w; level++);
    size = level*level*level;
    if (size != w) {
        av_log(ctx, AV_LOG_WARNING, "The Hald CLUT width does not match the level\n");
        return AVERROR_INVALIDDATA;
    }
    av_assert0(w == h && w == size);
    level *= level;
    if (level > MAX_LEVEL) {
        const int max_clut_level = sqrt(MAX_LEVEL);
        const int max_clut_size  = max_clut_level*max_clut_level*max_clut_level;
        av_log(ctx, AV_LOG_ERROR, "Too large Hald CLUT "
               "(maximum level is %d, or %dx%d CLUT)\n",
               max_clut_level, max_clut_size, max_clut_size);
        return AVERROR(EINVAL);
    }
    lut3d->lutsize = level;
    return 0;
}
