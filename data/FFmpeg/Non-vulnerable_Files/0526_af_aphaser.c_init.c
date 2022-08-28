AVFILTER_DEFINE_CLASS(aphaser);
static av_cold int init(AVFilterContext *ctx)
{
    AudioPhaserContext *s = ctx->priv;
    if (s->in_gain > (1 - s->decay * s->decay))
        av_log(ctx, AV_LOG_WARNING, "in_gain may cause clipping\n");
    if (s->in_gain / (1 - s->decay) > 1 / s->out_gain)
        av_log(ctx, AV_LOG_WARNING, "out_gain may cause clipping\n");
    return 0;
}
