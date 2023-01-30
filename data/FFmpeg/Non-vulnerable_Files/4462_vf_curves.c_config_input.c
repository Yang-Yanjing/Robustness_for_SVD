static int config_input(AVFilterLink *inlink)
{
    CurvesContext *curves = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    ff_fill_rgba_map(curves->rgba_map, inlink->format);
    curves->step = av_get_padded_bits_per_pixel(desc) >> 3;
    return 0;
}
