static av_cold void uninit(AVFilterContext *ctx)
{
    int p;
    EdgeDetectContext *edgedetect = ctx->priv;
    for (p = 0; p < edgedetect->nb_planes; p++) {
        struct plane_info *plane = &edgedetect->planes[p];
        av_freep(&plane->tmpbuf);
        av_freep(&plane->gradients);
        av_freep(&plane->directions);
    }
}
