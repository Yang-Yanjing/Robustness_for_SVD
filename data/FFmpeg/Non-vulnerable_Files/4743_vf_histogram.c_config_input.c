static int config_input(AVFilterLink *inlink)
{
    HistogramContext *h = inlink->dst->priv;
    h->desc  = av_pix_fmt_desc_get(inlink->format);
    h->ncomp = h->desc->nb_components;
    h->histogram_size = 1 << (h->desc->comp[0].depth_minus1 + 1);
    h->mult = h->histogram_size / 256;
    switch (inlink->format) {
    case AV_PIX_FMT_GBRP10:
    case AV_PIX_FMT_GBRP9:
    case AV_PIX_FMT_GBRAP:
    case AV_PIX_FMT_GBRP:
        h->bg_color = black_gbrp_color;
        h->fg_color = white_gbrp_color;
        break;
    default:
        h->bg_color = black_yuva_color;
        h->fg_color = white_yuva_color;
    }
    h->planeheight[1] = h->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, h->desc->log2_chroma_h);
    h->planeheight[0] = h->planeheight[3] = inlink->h;
    h->planewidth[1]  = h->planewidth[2]  = FF_CEIL_RSHIFT(inlink->w, h->desc->log2_chroma_w);
    h->planewidth[0]  = h->planewidth[3]  = inlink->w;
    return 0;
}
