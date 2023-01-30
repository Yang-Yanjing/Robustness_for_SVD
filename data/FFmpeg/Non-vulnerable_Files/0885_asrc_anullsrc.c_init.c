AVFILTER_DEFINE_CLASS(anullsrc);
static av_cold int init(AVFilterContext *ctx)
{
    ANullContext *null = ctx->priv;
    int ret;
    if ((ret = ff_parse_sample_rate(&null->sample_rate,
                                     null->sample_rate_str, ctx)) < 0)
        return ret;
    if ((ret = ff_parse_channel_layout(&null->channel_layout, NULL,
                                        null->channel_layout_str, ctx)) < 0)
        return ret;
    return 0;
}
