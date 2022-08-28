static int config_props(AVFilterLink *inlink)
{
    FlipContext *s = inlink->dst->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(inlink->format);
    const int hsub = pix_desc->log2_chroma_w;
    const int vsub = pix_desc->log2_chroma_h;
    av_image_fill_max_pixsteps(s->max_step, NULL, pix_desc);
    s->planewidth[0]  = s->planewidth[3]  = inlink->w;
    s->planewidth[1]  = s->planewidth[2]  = FF_CEIL_RSHIFT(inlink->w, hsub);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, vsub);
    return 0;
}
