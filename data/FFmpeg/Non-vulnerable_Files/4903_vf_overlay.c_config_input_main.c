static int config_input_main(AVFilterLink *inlink)
{
    OverlayContext *s = inlink->dst->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(inlink->format);
    av_image_fill_max_pixsteps(s->main_pix_step,    NULL, pix_desc);
    s->hsub = pix_desc->log2_chroma_w;
    s->vsub = pix_desc->log2_chroma_h;
    s->main_is_packed_rgb =
        ff_fill_rgba_map(s->main_rgba_map, inlink->format) >= 0;
    s->main_has_alpha = ff_fmt_is_in(inlink->format, alpha_pix_fmts);
    return 0;
}
