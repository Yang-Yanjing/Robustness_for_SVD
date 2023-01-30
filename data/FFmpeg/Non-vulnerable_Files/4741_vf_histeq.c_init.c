AVFILTER_DEFINE_CLASS(histeq);
static av_cold int init(AVFilterContext *ctx)
{
    HisteqContext *histeq = ctx->priv;
    av_log(ctx, AV_LOG_VERBOSE,
           "strength:%0.3f intensity:%0.3f antibanding:%d\n",
           histeq->strength, histeq->intensity, histeq->antibanding);
    return 0;
}
