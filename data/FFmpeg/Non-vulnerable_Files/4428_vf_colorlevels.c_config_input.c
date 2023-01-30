static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    ColorLevelsContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    s->nb_comp = desc->nb_components;
    s->bpp = (desc->comp[0].depth_minus1 + 1) >> 3;
    s->step = (av_get_padded_bits_per_pixel(desc) >> 3) / s->bpp;
    s->linesize = inlink->w * s->step;
    ff_fill_rgba_map(s->rgba_map, inlink->format);
    return 0;
}
