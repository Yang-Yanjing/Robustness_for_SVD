static int query_formats(AVFilterContext *ctx)
{
    ANullContext *null = ctx->priv;
    int64_t chlayouts[] = { null->channel_layout, -1 };
    int sample_rates[] = { null->sample_rate, -1 };
    ff_set_common_formats        (ctx, ff_all_formats(AVMEDIA_TYPE_AUDIO));
    ff_set_common_channel_layouts(ctx, avfilter_make_format64_list(chlayouts));
    ff_set_common_samplerates    (ctx, ff_make_format_list(sample_rates));
    return 0;
}
