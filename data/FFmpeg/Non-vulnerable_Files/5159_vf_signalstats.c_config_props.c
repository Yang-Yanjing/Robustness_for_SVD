static int config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    SignalstatsContext *s = ctx->priv;
    AVFilterLink *inlink = outlink->src->inputs[0];
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(outlink->format);
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    outlink->w = inlink->w;
    outlink->h = inlink->h;
    s->chromaw = FF_CEIL_RSHIFT(inlink->w, s->hsub);
    s->chromah = FF_CEIL_RSHIFT(inlink->h, s->vsub);
    s->fs = inlink->w * inlink->h;
    s->cfs = s->chromaw * s->chromah;
    s->nb_jobs   = FFMAX(1, FFMIN(inlink->h, ctx->graph->nb_threads));
    s->jobs_rets = av_malloc_array(s->nb_jobs, sizeof(*s->jobs_rets));
    if (!s->jobs_rets)
        return AVERROR(ENOMEM);
    s->frame_sat = alloc_frame(AV_PIX_FMT_GRAY8,  inlink->w, inlink->h);
    s->frame_hue = alloc_frame(AV_PIX_FMT_GRAY16, inlink->w, inlink->h);
    if (!s->frame_sat || !s->frame_hue)
        return AVERROR(ENOMEM);
    return 0;
}
