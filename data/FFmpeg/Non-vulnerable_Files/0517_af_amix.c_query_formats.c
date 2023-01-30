static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *formats = NULL;
    AVFilterChannelLayouts *layouts;
    int ret;
    layouts = ff_all_channel_layouts();
    if (!layouts)
        return AVERROR(ENOMEM);
    ff_add_format(&formats, AV_SAMPLE_FMT_FLT);
    ff_add_format(&formats, AV_SAMPLE_FMT_FLTP);
    ret = ff_set_common_formats(ctx, formats);
    if (ret < 0)
        return ret;
    ret = ff_set_common_channel_layouts(ctx, layouts);
    if (ret < 0)
        return ret;
    return ff_set_common_samplerates(ctx, ff_all_samplerates());
}
