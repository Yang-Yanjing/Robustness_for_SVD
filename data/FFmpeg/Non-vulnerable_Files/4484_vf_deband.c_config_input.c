static int config_input(AVFilterLink *inlink)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    AVFilterContext *ctx = inlink->dst;
    DebandContext *s = ctx->priv;
    const float direction = s->direction;
    const int range = s->range;
    int x, y;
    s->nb_components = desc->nb_components;
    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->planewidth[1]  = s->planewidth[2]  = FF_CEIL_RSHIFT(inlink->w, desc->log2_chroma_w);
    s->planewidth[0]  = s->planewidth[3]  = inlink->w;
    s->deband = desc->comp[0].depth_minus1 > 7 ? deband_16_c : deband_8_c;
    s->thr[0] = ((1 << (desc->comp[0].depth_minus1 + 1)) - 1) * s->threshold[0];
    s->thr[1] = ((1 << (desc->comp[1].depth_minus1 + 1)) - 1) * s->threshold[1];
    s->thr[2] = ((1 << (desc->comp[2].depth_minus1 + 1)) - 1) * s->threshold[2];
    s->thr[3] = ((1 << (desc->comp[3].depth_minus1 + 1)) - 1) * s->threshold[3];
    s->x_pos = av_malloc(s->planewidth[0] * s->planeheight[0] * sizeof(*s->x_pos));
    s->y_pos = av_malloc(s->planewidth[0] * s->planeheight[0] * sizeof(*s->y_pos));
    if (!s->x_pos || !s->y_pos)
        return AVERROR(ENOMEM);
    for (y = 0; y < s->planeheight[0]; y++) {
        for (x = 0; x < s->planewidth[0]; x++) {
            const float r = frand(x, y);
            const float dir = direction < 0 ? -direction : r * direction;
            const int dist = range < 0 ? -range : r * range;
            s->x_pos[y * s->planeheight[0] + x] = cosf(dir) * dist;
            s->y_pos[y * s->planeheight[0] + x] = sinf(dir) * dist;
        }
    }
    return 0;
}
