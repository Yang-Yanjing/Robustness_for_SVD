static int query_formats(AVFilterContext *ctx)
{
    static const int sample_rates[] = { 44100, -1 };
    AVFilterFormats *formats = NULL;
    AVFilterChannelLayouts *layout = NULL;
    ff_add_format(&formats, AV_SAMPLE_FMT_S16);
    ff_set_common_formats(ctx, formats);
    ff_add_channel_layout(&layout, AV_CH_LAYOUT_STEREO);
    ff_set_common_channel_layouts(ctx, layout);
    ff_set_common_samplerates(ctx, ff_make_format_list(sample_rates));
    return 0;
}
