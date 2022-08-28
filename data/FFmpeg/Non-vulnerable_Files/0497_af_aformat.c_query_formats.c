static int query_formats(AVFilterContext *ctx)
{
    AFormatContext *s = ctx->priv;
    int ret;
    ret = ff_set_common_formats(ctx, s->formats ? s->formats :
                                            ff_all_formats(AVMEDIA_TYPE_AUDIO));
    if (ret < 0)
        return ret;
    ret = ff_set_common_samplerates(ctx, s->sample_rates ? s->sample_rates :
                                                     ff_all_samplerates());
    if (ret < 0)
        return ret;
    return ff_set_common_channel_layouts(ctx, s->channel_layouts ? s->channel_layouts :
                                                            ff_all_channel_counts());
}
