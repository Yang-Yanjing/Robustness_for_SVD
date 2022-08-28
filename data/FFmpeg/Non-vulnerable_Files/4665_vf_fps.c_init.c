AVFILTER_DEFINE_CLASS(fps);
static av_cold int init(AVFilterContext *ctx)
{
    FPSContext *s = ctx->priv;
    if (!(s->fifo = av_fifo_alloc_array(2, sizeof(AVFrame*))))
        return AVERROR(ENOMEM);
    s->first_pts    = AV_NOPTS_VALUE;
    av_log(ctx, AV_LOG_VERBOSE, "fps=%d/%d\n", s->framerate.num, s->framerate.den);
    return 0;
}
