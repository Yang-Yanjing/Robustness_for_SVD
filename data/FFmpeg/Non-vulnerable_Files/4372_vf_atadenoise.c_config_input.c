static int config_input(AVFilterLink *inlink)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    AVFilterContext *ctx = inlink->dst;
    ATADenoiseContext *s = ctx->priv;
    int depth;
    s->nb_planes = desc->nb_components;
    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->planewidth[1]  = s->planewidth[2]  = FF_CEIL_RSHIFT(inlink->w, desc->log2_chroma_w);
    s->planewidth[0]  = s->planewidth[3]  = inlink->w;
    depth = desc->comp[0].depth_minus1 + 1;
    if (depth == 8)
        s->filter_slice = filter_slice8;
    else
        s->filter_slice = filter_slice16;
    s->thra[0] = s->fthra[0] * (1 << depth) - 1;
    s->thra[1] = s->fthra[1] * (1 << depth) - 1;
    s->thra[2] = s->fthra[2] * (1 << depth) - 1;
    s->thrb[0] = s->fthrb[0] * (1 << depth) - 1;
    s->thrb[1] = s->fthrb[1] * (1 << depth) - 1;
    s->thrb[2] = s->fthrb[2] * (1 << depth) - 1;
    return 0;
}
