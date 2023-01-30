static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *formats = NULL;
    AVFilterChannelLayouts *layout = NULL;
    int i;
    ff_add_format(&formats, AV_SAMPLE_FMT_FLT);
    ff_set_common_formats(ctx, formats);
    ff_add_channel_layout(&layout, AV_CH_LAYOUT_STEREO);
    ff_set_common_channel_layouts(ctx, layout);
    formats = NULL;
    for (i = 0; i < FF_ARRAY_ELEMS(freqinfos); i++)
        ff_add_format(&formats, freqinfos[i].sample_rate);
    ff_set_common_samplerates(ctx, formats);
    return 0;
}
