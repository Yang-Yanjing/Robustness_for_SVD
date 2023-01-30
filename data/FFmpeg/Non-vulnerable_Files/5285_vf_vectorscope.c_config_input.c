static int config_input(AVFilterLink *inlink)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    VectorscopeContext *s = inlink->dst->priv;
    s->is_yuv = !(desc->flags & AV_PIX_FMT_FLAG_RGB);
    if (s->mode == GRAY && s->is_yuv)
        s->pd = 0;
    else {
        if ((s->x == 1 && s->y == 2) || (s->x == 2 && s->y == 1))
            s->pd = 0;
        else if ((s->x == 0 && s->y == 2) || (s->x == 2 && s->y == 0))
            s->pd = 1;
        else if ((s->x == 0 && s->y == 1) || (s->x == 1 && s->y == 0))
            s->pd = 2;
    }
    switch (inlink->format) {
    case AV_PIX_FMT_GBRAP:
    case AV_PIX_FMT_GBRP:
        s->bg_color = black_gbrp_color;
        break;
    default:
        s->bg_color = black_yuva_color;
    }
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->planewidth[1]  = s->planewidth[2]  = FF_CEIL_RSHIFT(inlink->w, desc->log2_chroma_w);
    s->planewidth[0]  = s->planewidth[3]  = inlink->w;
    return 0;
}
