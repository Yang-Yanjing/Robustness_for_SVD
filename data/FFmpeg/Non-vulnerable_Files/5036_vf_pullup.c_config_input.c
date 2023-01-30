static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    PullupContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int mp = s->metric_plane;
    s->nb_planes = av_pix_fmt_count_planes(inlink->format);
    if (mp + 1 > s->nb_planes) {
        av_log(ctx, AV_LOG_ERROR, "input format does not have such plane\n");
        return AVERROR(EINVAL);
    }
    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->planewidth[1]  = s->planewidth[2]  = FF_CEIL_RSHIFT(inlink->w, desc->log2_chroma_w);
    s->planewidth[0]  = s->planewidth[3]  = inlink->w;
    s->metric_w      = (s->planewidth[mp]  - ((s->junk_left + s->junk_right)  << 3)) >> 3;
    s->metric_h      = (s->planeheight[mp] - ((s->junk_top  + s->junk_bottom) << 1)) >> 3;
    s->metric_offset = (s->junk_left << 3) + (s->junk_top << 1) * s->planewidth[mp];
    s->metric_length = s->metric_w * s->metric_h;
    av_log(ctx, AV_LOG_DEBUG, "w: %d h: %d\n", s->metric_w, s->metric_h);
    av_log(ctx, AV_LOG_DEBUG, "offset: %d length: %d\n", s->metric_offset, s->metric_length);
    s->head = make_field_queue(s, 8);
    if (!s->head)
        return AVERROR(ENOMEM);
    s->diff = diff_c;
    s->comb = comb_c;
    s->var  = var_c;
    if (ARCH_X86)
        ff_pullup_init_x86(s);
    return 0;
}
