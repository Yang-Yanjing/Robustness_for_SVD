AVFILTER_DEFINE_CLASS(dynaudnorm);
static av_cold int init(AVFilterContext *ctx)
{
    DynamicAudioNormalizerContext *s = ctx->priv;
    if (!(s->filter_size & 1)) {
        av_log(ctx, AV_LOG_ERROR, "filter size %d is invalid. Must be an odd value.\n", s->filter_size);
        return AVERROR(EINVAL);
    }
    return 0;
}
