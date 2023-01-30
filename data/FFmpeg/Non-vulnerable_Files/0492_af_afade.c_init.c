AVFILTER_DEFINE_CLASS(afade);
static av_cold int init(AVFilterContext *ctx)
{
    AudioFadeContext *s = ctx->priv;
    if (INT64_MAX - s->nb_samples < s->start_sample)
        return AVERROR(EINVAL);
    return 0;
}
