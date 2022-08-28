AVFILTER_DEFINE_CLASS(silenceremove);
static av_cold int init(AVFilterContext *ctx)
{
    SilenceRemoveContext *s = ctx->priv;
    if (s->stop_periods < 0) {
        s->stop_periods = -s->stop_periods;
        s->restart = 1;
    }
    return 0;
}
