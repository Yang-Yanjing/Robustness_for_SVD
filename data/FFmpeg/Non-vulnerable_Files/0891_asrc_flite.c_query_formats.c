static int query_formats(AVFilterContext *ctx)
{
    FliteContext *flite = ctx->priv;
    AVFilterChannelLayouts *chlayouts = NULL;
    int64_t chlayout = av_get_default_channel_layout(flite->wave->num_channels);
    AVFilterFormats *sample_formats = NULL;
    AVFilterFormats *sample_rates = NULL;
    ff_add_channel_layout(&chlayouts, chlayout);
    ff_set_common_channel_layouts(ctx, chlayouts);
    ff_add_format(&sample_formats, AV_SAMPLE_FMT_S16);
    ff_set_common_formats(ctx, sample_formats);
    ff_add_format(&sample_rates, flite->wave->sample_rate);
    ff_set_common_samplerates (ctx, sample_rates);
    return 0;
}
