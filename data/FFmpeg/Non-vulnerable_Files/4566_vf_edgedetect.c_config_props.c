static int config_props(AVFilterLink *inlink)
{
    int p;
    AVFilterContext *ctx = inlink->dst;
    EdgeDetectContext *edgedetect = ctx->priv;
    edgedetect->nb_planes = inlink->format == AV_PIX_FMT_GRAY8 ? 1 : 3;
    for (p = 0; p < edgedetect->nb_planes; p++) {
        struct plane_info *plane = &edgedetect->planes[p];
        plane->tmpbuf     = av_malloc(inlink->w * inlink->h);
        plane->gradients  = av_calloc(inlink->w * inlink->h, sizeof(*plane->gradients));
        plane->directions = av_malloc(inlink->w * inlink->h);
        if (!plane->tmpbuf || !plane->gradients || !plane->directions)
            return AVERROR(ENOMEM);
    }
    return 0;
}
